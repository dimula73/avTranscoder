#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP

#include "PixelProperties.hpp"

#include <AvTranscoder/properties/StreamProperties.hpp>
#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/IProgress.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <string>
#include <vector>
#include <utility>

namespace avtranscoder
{

class AvExport VideoProperties : public StreamProperties
{
public:
    VideoProperties(const FormatContext& formatContext, const size_t index, IProgress& progress,
                    const EAnalyseLevel level = eAnalyseLevelFirstGop);

    std::string getProfileName() const;
    std::string getColorTransfert() const;
    std::string getColorspace() const;
    std::string getColorRange() const;
    std::string getColorPrimaries() const;
    std::string getChromaSampleLocation() const;
    std::string getFieldOrder() const;

    PixelProperties& getPixelProperties() { return _pixelProperties; }

    int64_t getStartTimecode() const;
    std::string getStartTimecodeString() const;

    Rational getSar() const; // sample/pixel aspect ratio
    Rational getDar() const; // display aspect ratio

    size_t getBitRate() const; ///< in bits/s
    size_t getMaxBitRate() const;
    size_t getMinBitRate() const;
    size_t getNbFrames() const;
    size_t getTicksPerFrame() const;
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getDtgActiveFormat() const;
    size_t getReferencesFrames() const;
    int getProfile() const;
    int getLevel() const;

    /**
     * @brief Corresponds to the 'fps' returned by ffprobe.
     * fps = the average framerate that has come from the AVStream
     * tbn = the time base in AVStream that has come from the AVStream
     * tbc = the time base in AVCodecContext for the codec used for a particular stream
     * tbr = tbr is guessed from the video stream and is the value users want to see when they look for the video frame rate
     */
    float getFps() const;

    bool hasBFrames() const;
    // bool isClosedGop() const;

    //@{
    // Warning: Can acces these data when analyse first gop
    // @see EAnalyseLevel
    // @see analyseGopStructure
    bool isInterlaced() const { return _isInterlaced; }
    bool isTopFieldFirst() const { return _isTopFieldFirst; }
    /**
     * @return the distance between two nearest I frame
     */
    size_t getGopSize() const { return _gopSize; }
    std::vector<std::pair<char, int> > getGopStructure() const { return _gopStructure; }
//@}

#ifndef SWIG
    AVCodecContext& getAVCodecContext() { return *_codecContext; }
    const PixelProperties& getPixelProperties() const { return _pixelProperties; }
#endif

    PropertyVector& fillVector(PropertyVector& data) const;

private:
    /**
     *  @brief frame type / is key frame
     *  @param progress: callback to get analysis progression
     */
    void analyseGopStructure(IProgress& progress);

#ifndef SWIG
    template <typename T>
    void addProperty(PropertyVector& dataVector, const std::string& key, T (VideoProperties::*getter)(void) const) const
    {
        try
        {
            detail::add(dataVector, key, (this->*getter)());
        }
        catch(const std::exception& e)
        {
            detail::add(dataVector, key, detail::propertyValueIfError);
        }
    }
#endif

private:
    /**
     * @brief Level of analysis asked.
     */
    EAnalyseLevel _levelAnalysis;

    /**
     * @brief All the pixel properties contained in this stream.
     */
    PixelProperties _pixelProperties;

    //@{
    // Can acces these data when analyse first gop
    bool _isInterlaced;
    bool _isTopFieldFirst;
    size_t _gopSize;
    std::vector<std::pair<char, int> > _gopStructure;  ///< picture type, encoded frame size in bytes
    //@}

    /**
     * @brief GOP timecode of the first frame
     * @note  AVCodecContext stores the GOP timecode of the last decoded frame
     */
    int64_t _firstGopTimeCode;
};

#ifndef SWIG
AvExport std::ostream& operator<<(std::ostream& flux, const StreamProperties& streamProperties);
#endif
}

#endif
