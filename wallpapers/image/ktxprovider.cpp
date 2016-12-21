/***************************************************************************
 *   Copyright 2016 Fredrik Höglund <fredrik@kde.org>                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include <epoxy/gl.h>

#include "ktxprovider.h"
#include "ktxreader.h"

#include <QDebug>
#include <QFile>
#include <QGlobalStatic>
#include <QOpenGLContext>
#include <QSGTexture>


struct ContextDataSingleton
{
    ContextDataSingleton() {
        QOpenGLContext * const ctx = QOpenGLContext::currentContext();
        version = (ctx->format().majorVersion() << 8) | ctx->format().minorVersion();

        if (!ctx->isOpenGLES()) {
            haveTextureStorage         = version >= 0x0402 || ctx->hasExtension("GL_ARB_texture_storage");
            haveTextureCompressionBptc = version >= 0x0402 || ctx->hasExtension("GL_ARB_texture_compression_bptc");
            haveInternalFormatQuery2   = version >= 0x0403 || ctx->hasExtension("GL_ARB_internalformat_query2");
            haveBufferStorage          = version >= 0x0404 || ctx->hasExtension("GL_ARB_buffer_storage");
        } else {
            haveTextureStorage         = version >= 0x0301 || ctx->hasExtension("GL_EXT_texture_storage");
            haveTextureCompressionBptc = false;
            haveInternalFormatQuery2   = false;
            haveBufferStorage          = ctx->hasExtension("GL_EXT_buffer_storage");
        }
    }

    uint32_t version;
    bool haveTextureStorage;
    bool haveTextureCompressionBptc;
    bool haveInternalFormatQuery2;
    bool haveBufferStorage;
};

Q_GLOBAL_STATIC(ContextDataSingleton, contextData)



// ------------------------------------------------------------------



class KtxTexture : public QSGTexture
{
public:
    KtxTexture(GLenum internalFormat, const QSize &size, const QVector<size_t> &mipLevelByteCounts, const QByteArray &data);
    ~KtxTexture();

    void bind() override final;
    bool hasAlphaChannel() const override final { return m_hasAlphaChannel; }
    bool hasMipmaps() const override final { return m_mipLevelByteCounts.count() > 1; }
    int textureId() const override final { return m_texture; }
    QSize textureSize() const override final { return m_baseLevelSize; }

protected:
    bool isFormatSupported(GLenum internalFormat) const;
    bool formatHasAlpha(GLenum internalFormat) const;
    bool loadTexture();

private:
    GLuint m_texture;
    GLenum m_internalFormat;
    QSize m_baseLevelSize;
    const QVector<size_t> m_mipLevelByteCounts;
    const QByteArray m_data;
    bool m_hasAlphaChannel;
};

KtxTexture::KtxTexture(GLenum internalFormat, const QSize &baseLevelSize, const QVector<size_t> &mipLevelByteCounts, const QByteArray &data)
    : m_texture(0),
      m_internalFormat(internalFormat),
      m_baseLevelSize(baseLevelSize),
      m_mipLevelByteCounts(mipLevelByteCounts),
      m_data(data)
{
}

KtxTexture::~KtxTexture()
{
    glDeleteTextures(1, &m_texture);
}

bool KtxTexture::isFormatSupported(GLenum internalFormat) const
{
    // If we have ARB_internalformat_query2 we can ask the driver directly
    if (contextData->haveInternalFormatQuery2) {
        GLint value;
        glGetInternalformativ(GL_TEXTURE_2D, internalFormat, GL_INTERNALFORMAT_SUPPORTED, 1, &value);
        return value == GL_TRUE;
    }

    switch (internalFormat) {
    case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:
        return contextData->haveTextureCompressionBptc;

    default:
       return false;
    }
}

bool KtxTexture::formatHasAlpha(GLenum internalFormat) const
{
    // If we have ARB_internalformat_query2 we can ask the driver directly
    if (contextData->haveInternalFormatQuery2) {
        GLint value;
        glGetInternalformativ(GL_TEXTURE_2D, internalFormat, GL_INTERNALFORMAT_ALPHA_SIZE, 1, &value);
        return value > 0;
    }

    switch (internalFormat) {
    case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
        return true;

    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:
        return false;

    default:
       return false;
    }
}

bool KtxTexture::loadTexture()
{
    if (!isFormatSupported(m_internalFormat)) {
        qDebug() << "The implementation does not support the texture format";
        return false;
    }

    m_hasAlphaChannel = formatHasAlpha(m_internalFormat);

    const int width0 = textureSize().width();
    const int height0 = textureSize().height();
    const int mipLevelCount = m_mipLevelByteCounts.count();

    const bool haveBufferStorage = contextData->haveBufferStorage;
    const bool haveTextureStorage = contextData->haveTextureStorage;

    // Create the texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    if (haveTextureStorage)
        glTexStorage2D(GL_TEXTURE_2D, mipLevelCount, m_internalFormat, width0, height0);

    setFiltering(QSGTexture::Linear);
    setMipmapFiltering(QSGTexture::Linear);
    setHorizontalWrapMode(QSGTexture::ClampToEdge);
    setVerticalWrapMode(QSGTexture::ClampToEdge);

    updateBindOptions(true /*force*/);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevelCount - 1);

    // Save the PBO binding and the unpack alignment so we can restore them later
    GLuint prevBinding = 0;
    GLuint prevAlignment = 0;

    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, (GLint *) &prevBinding);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, (GLint *) &prevAlignment);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Create a staging buffer
    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);

    if (haveBufferStorage) {
        glBufferStorage(GL_PIXEL_UNPACK_BUFFER, m_data.size(), m_data.constData(), GL_CLIENT_STORAGE_BIT);
    } else {
        glBufferData(GL_PIXEL_UNPACK_BUFFER, m_data.size(), m_data.constData(), GL_STREAM_COPY);
    }

    intptr_t offset = 0;

    for (int i = 0; i < mipLevelCount; i++) {
        const uint32_t width  = width0 >> i;
        const uint32_t height = height0 >> i;
        const uint32_t byteCount = m_mipLevelByteCounts[i];

        // Copy the miplevel from the staging buffer to the texture
        if (haveTextureStorage) {
            glCompressedTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height,
                                      m_internalFormat, byteCount, (const void *) offset);
        } else {
            glCompressedTexImage2D(GL_TEXTURE_2D, i, m_internalFormat,
                                   width, height, 0, byteCount, (const void *) offset);
        }

        // Align to four bytes
        offset += (byteCount + 3) & ~3;
    }

    // Restore the unpack alignment
    if (prevAlignment != 4)
        glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);

    // Restore the pixel unpack buffer binding and delete the staging buffer
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, prevBinding);
    glDeleteBuffers(1, &pbo);

    return true;
}

void KtxTexture::bind()
{
    if (!m_texture) {
        loadTexture();
    }

    glBindTexture(GL_TEXTURE_2D, m_texture);
}



// ------------------------------------------------------------------



class KtxTextureFactory : public QQuickTextureFactory
{
public:
    KtxTextureFactory(GLenum internalFormat, const QSize &size, const QVector<size_t> &mipLevelByteCounts, const QByteArray &data);

    QSGTexture *createTexture(QQuickWindow *window) const override final;

    QSize textureSize() const override final { return m_size; }
    int textureByteCount() const override final { return m_data.size(); }

private:
    GLenum m_internalFormat;
    QSize m_size;
    QVector<size_t> m_mipLevelByteCounts;
    QByteArray m_data;
};


KtxTextureFactory::KtxTextureFactory(GLenum internalFormat, const QSize &size, const QVector<size_t> &mipLevelByteCounts, const QByteArray &data)
    : m_internalFormat(internalFormat),
      m_size(size),
      m_mipLevelByteCounts(mipLevelByteCounts),
      m_data(data)
{
}

QSGTexture *KtxTextureFactory::createTexture(QQuickWindow *window) const
{
    Q_UNUSED(window)

    KtxTexture *texture = new KtxTexture(m_internalFormat, m_size, m_mipLevelByteCounts, m_data);

    return texture;
}



// ------------------------------------------------------------------



KtxProvider::KtxProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Texture)
{
}

QQuickTextureFactory *KtxProvider::requestTexture(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)

    KtxReader reader(QStringLiteral("/") + id);
    if (!reader.canRead())
        return nullptr;

    auto data = reader.data();
    if (data.isEmpty())
        return nullptr;

    *size = reader.size();

    return new KtxTextureFactory(reader.internalFormat(), reader.size(), reader.mipLevelByteCounts(), data);
}

