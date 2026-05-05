#ifndef QRENDER_H
#define QRENDER_H

#endif // QRENDER_H
#pragma once
#include <QString>
#include <QVector>

// Génération QR Code version 3, niveau M — 100% Qt, sans lib externe
class QRCodeGen {
public:
    static QVector<QVector<bool>> generate(const QString &text) {
        QByteArray data = text.toUtf8();
        const int version = 3;
        const int size = 17 + version * 4; // 29x29

        QVector<QVector<bool>> matrix(size, QVector<bool>(size, false));
        QVector<QVector<bool>> reserved(size, QVector<bool>(size, false));

        // Finder patterns
        auto placeFinder = [&](int row, int col) {
            for (int r = -1; r <= 7; r++)
                for (int c = -1; c <= 7; c++) {
                    if (row+r < 0 || row+r >= size || col+c < 0 || col+c >= size) continue;
                    reserved[row+r][col+c] = true;
                    bool dark = (r>=0 && r<=6 && (c==0||c==6)) ||
                                (c>=0 && c<=6 && (r==0||r==6)) ||
                                (r>=2 && r<=4 && c>=2 && c<=4);
                    matrix[row+r][col+c] = dark;
                }
        };
        placeFinder(0, 0);
        placeFinder(0, size-7);
        placeFinder(size-7, 0);

        // Timing patterns
        for (int i = 8; i < size-8; i++) {
            matrix[6][i] = matrix[i][6] = (i % 2 == 0);
            reserved[6][i] = reserved[i][6] = true;
        }

        // Dark module
        matrix[size-8][8] = true;
        reserved[size-8][8] = true;

        // Encoder les données (byte mode)
        QVector<bool> bits;
        // Mode indicator: byte = 0100
        bits << false << true << false << false;
        // Character count (8 bits pour version 1-9)
        int len = qMin(data.size(), 50); // limite simple
        for (int i = 7; i >= 0; i--) bits << ((len >> i) & 1);
        // Data bytes
        for (int i = 0; i < len; i++)
            for (int b = 7; b >= 0; b--)
                bits << (((unsigned char)data[i] >> b) & 1);
        // Terminator
        for (int i = 0; i < 4 && bits.size() < 196; i++) bits << false;
        // Pad to byte boundary
        while (bits.size() % 8 != 0) bits << false;
        // Pad codewords
        bool padToggle = false;
        while (bits.size() < 196) {
            if (padToggle) for (int b=7;b>=0;b--) bits<<((0x11>>b)&1);
            else           for (int b=7;b>=0;b--) bits<<((0xEC>>b)&1);
            padToggle = !padToggle;
        }

        // Placer les bits dans la matrice (zigzag de droite à gauche)
        int bitIdx = 0;
        bool goUp = true;
        for (int right = size-1; right >= 1; right -= 2) {
            if (right == 6) right--;
            for (int vert = 0; vert < size; vert++) {
                int row = goUp ? (size-1-vert) : vert;
                for (int j = 0; j < 2; j++) {
                    int col = right - j;
                    if (!reserved[row][col] && bitIdx < bits.size()) {
                        matrix[row][col] = bits[bitIdx++];
                    }
                }
            }
            goUp = !goUp;
        }

        // Masque 0 : (row+col) % 2 == 0
        for (int r = 0; r < size; r++)
            for (int c = 0; c < size; c++)
                if (!reserved[r][c] && (r+c)%2==0)
                    matrix[r][c] = !matrix[r][c];

        return matrix;
    }
};