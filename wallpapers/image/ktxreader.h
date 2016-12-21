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

#ifndef KTXHEADER_H
#define KTXHEADER_H

#include <QByteArray>
#include <QFile>
#include <QVector>

class QSize;
typedef unsigned int GLenum;


/**
 * The KtxReader class provides an interface for reading Khronos Texture files.
 */
class KtxReader
{
    struct KtxHeader {
        uint32_t endianness;
        uint32_t glType;
        uint32_t glTypeSize;
        uint32_t glFormat;
        uint32_t glInternalFormat;
        uint32_t glBaseInternalFormat;
        uint32_t pixelWidth;
        uint32_t pixelHeight;
        uint32_t pixelDepth;
        uint32_t numberOfArrayElements;
        uint32_t numberOfFaces;
        uint32_t numberOfMipmapLevels;
        uint32_t bytesOfKeyValueData;
    };

public:
    enum Mode { HeaderOnly, HeaderAndImage };

    /**
     * Creates a KtxReader.
     */
    KtxReader(const QString &fileName, Mode = HeaderAndImage);

    /**
     * Destroys the KtxReader.
     */
    ~KtxReader();

    /**
     * Returns true if the header is valid, and false otherwise.
     *
     * Note that data() may fail if the image data is corrupt, even
     * if canRead() returns true.
     */
    bool canRead() const;

    /**
     * Returns the size of the base level, or an invalid size on failure.
     */
    QSize size() const;

    /**
     * Returns the number of mipmap levels in the image.
     */
    int mipLevelCount() const;

    /**
     * Returns the internal format of the texture.
     */
    GLenum internalFormat() const;

    /**
     * Returns the raw image data.
     *
     * Each mip-level is stored sequentially in the buffer, with
     * the first byte of each level aligned to four bytes.
     *
     * Returns an empty QByteArray on failure.
     */
    QByteArray data();

    /**
     * Returns the byte size of each miplevel as a vector.
     *
     * Returns an empty QVector on failure.
     */
    QVector<size_t> mipLevelByteCounts();

protected:
    /**
     * @internal
     */
    bool readImageData();

private:
    QFile m_file;
    uint8_t *m_map;
    intptr_t m_pos;
    KtxHeader m_header;
    QByteArray m_data;
    QVector<size_t> m_mipLevelByteCounts;
    //QMap<QString, QByteArray> m_metaData;
    bool m_headerValid = false;
    bool m_imageLoaded = false;
    bool m_byteSwap = false;
};

#endif
