#ifndef NETWORKOUTPUTSTREAM_H
#define NETWORKOUTPUTSTREAM_H
#include "outputstream.h"
#include "util.h"

class NetworkOutputStream: public OutputStream
{
    public:
        /** Default constructor */
        NetworkOutputStream();
        NetworkOutputStream(int socket);
        NetworkOutputStream(const NetworkOutputStream& origin);

        int open(const char* hostname, int port);
        /** Default destructor */
        virtual ~NetworkOutputStream();

        virtual void writeChar (unsigned char v);
        /* Write 2 bytes in the output (little endian order) */
        virtual void writeShortInt (short int v);
        /* Write 4 bytes in the output (little endian order) */
        virtual void writeInt (int v);
        /* Write 4 bytes in the output (little endian order) */
        virtual void writeLong (long v);
        /* Write 8 bytes in the output (little endian order) */
        virtual void writeLong64 (__LONG64 v);
        /* Write a 4 byte float in the output */
        virtual void writeFloatIEEE (float v);
        /* Write a 8 byte double in the output */
        virtual void writeDoubleIEEE (double v);
        /* Write a char */
        virtual void writeChars(const char* text, int len);
        virtual void writeString(const std::string& text);
        void closeStream();

        int setNonblocking();
        int disableNagle();

    protected:
    private:
        int _socket;
        Logger* _logger;
};

#endif // NETWORKOUTPUTSTREAM_H
