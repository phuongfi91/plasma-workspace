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

#include "ktxreader.h"

#include <QDebug>
#include <QFile>
#include <QSize>
#include <QString>


static constexpr uint32_t swap(uint32_t v)
{
    return (v << 24) | (v >> 24) | ((v & 0x0000ff00) << 8) | ((v & 0x00ff0000) >> 8);
}


KtxReader::KtxReader(const QString &fileName, KtxReader::Mode mode)
    : m_file(fileName),
      m_pos(0)
{
    const uint8_t magic[12] = {
        0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
    };

    if (!m_file.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open" << fileName << "for reading";
        return;
    }

    if (m_file.size() < sizeof(magic) + sizeof(KtxHeader)) {
        qDebug() << "Unexpected end of file in" << fileName;
    }

    size_t size = (mode == HeaderOnly) ? 12 + sizeof(KtxHeader) : m_file.size();

    if (!(m_map = m_file.map(0, size))) {
        qDebug() << "Failed to map" << fileName;
        return;
    }

    // Verify that the file is a KTX file
    if (memcmp(m_map + m_pos, magic, sizeof(magic)) != 0) {
        qDebug() << fileName << "is not a valid KTX file.";
        return;
    }

    // Read the rest of the header
    m_pos += sizeof(magic);

    memcpy(&m_header, m_map + m_pos, sizeof(KtxHeader));
    m_pos += sizeof(KtxHeader);

    // Check if the endianess is valid
    if (m_header.endianness != 0x01020304 && m_header.endianness != 0x04030201) {
        qDebug() << "Invalid endianess in" << fileName;
        return;
    }

    m_byteSwap = m_header.endianness != 0x04030201;

    // Byte swap the header if necessary
    if (m_byteSwap) {
        uint32_t *data = reinterpret_cast<uint32_t *>(&m_header);
        for (unsigned int i = 0; i < sizeof(KtxHeader) / sizeof(uint32_t); i++) {
            data[i] = swap(data[i]);
        }
    }

    if (m_header.pixelDepth > 1) {
        qDebug() << "3D textures are not supported";
        return;
    }

    if (m_header.numberOfArrayElements > 0) {
        qDebug() << "Array textures are not supported";
        return;
    }

    if (m_header.numberOfFaces > 1) {
        qDebug() << "Cube map textures are not supported";
        return;
    }

    if (m_header.glType != 0) {
        // Uncompressed format
        qDebug() << "Only compressed textures are supported at this time";
        return;
    }

    // glTypeSize must be 1 for compressed textures
    if (m_header.glTypeSize != 1) {
        qDebug() << "Invalid type size";
        return;
    }

    // glFormat must be 0 for compressed textures
    if (m_header.glFormat != 0) {
        qDebug() << "Invalid GL format";
        return;
    }

    if (m_file.size() < m_pos + m_header.bytesOfKeyValueData) {
        qDebug() << "Unexpected end of file in" << fileName;
        return;
    }

    // Skip the meta data
    m_pos += m_header.bytesOfKeyValueData;

    m_headerValid = true;
}

KtxReader::~KtxReader()
{
}

bool KtxReader::canRead() const
{
    return m_headerValid;
}

QSize KtxReader::size() const
{
    return QSize(m_header.pixelWidth, m_header.pixelHeight);
}

int KtxReader::mipLevelCount() const
{
    return m_mipLevelByteCounts.count();
}

GLenum KtxReader::internalFormat() const
{
    return m_header.glInternalFormat;
}

QByteArray KtxReader::data()
{
    if (m_headerValid && !m_imageLoaded) {
        readImageData();
        m_imageLoaded = true;
    }

    return m_data;
}

QVector<size_t> KtxReader::mipLevelByteCounts()
{
    if (m_headerValid && !m_imageLoaded) {
        readImageData();
        m_imageLoaded = true;
    }

    return m_mipLevelByteCounts;
}

bool KtxReader::readImageData()
{
    const ptrdiff_t size = m_file.size() - m_pos - m_header.numberOfMipmapLevels * sizeof(uint32_t);

    if (size < 0) {
        qDebug() << "Unexpected end of file";
        return false;
    }

    QByteArray imageData;
    imageData.resize(size);

    QVector<size_t> mipLevelByteCounts;
    mipLevelByteCounts.resize(m_header.numberOfMipmapLevels);

    uint8_t *dst = (uint8_t *) imageData.data();

    for (unsigned int i = 0; i < m_header.numberOfMipmapLevels; i++) {
        // Read the imageSize field
        if (m_file.size() < m_pos + sizeof(uint32_t)) {
            qDebug() << "Unexpected end of file";
            return false;
        }

        uint32_t imageSize = *(uint32_t *) (m_map + m_pos);
        m_pos += sizeof(uint32_t);

        if (m_byteSwap)
            imageSize = swap(imageSize);

        if (m_file.size() < m_pos + imageSize) {
            qDebug() << "Unexpected end of file";
            return false;
        }

        // Copy the miplevel data to the memory buffer
        memcpy(dst, m_map + m_pos, imageSize);

        // Align the size to four bytes and advance the data pointers
        const uint32_t alignedSize = (imageSize + 3) & ~3;

        dst += alignedSize;
        m_pos += alignedSize;

        mipLevelByteCounts[i] = imageSize;
    }

    m_data = imageData;
    m_mipLevelByteCounts = mipLevelByteCounts;

    return true;
}

