#ifndef HEADER_GUARD_IEXTERNALDECODER_H
#define HEADER_GUARD_IEXTERNALDECODER_H

#include "../FFmpegHeaders.hpp"

namespace JAZZROS {

class FFmpegIExternalDecoder
{

public:

    virtual                     ~FFmpegIExternalDecoder() {}

    virtual AVCodec*            openCodec(AVCodecContext* pCodec, AVStream *) = 0;

    virtual const int           Convert(AVFrame * pSrcFrame, AVFrame * pDstFrame) = 0;

    virtual const AVPixelFormat GetOutPixFmt(void) const = 0;

}; // FFmpegIExternalDecoder

} // namespace JAZZROS

#endif // HEADER_GUARD_IEXTERNALDECODER_H
