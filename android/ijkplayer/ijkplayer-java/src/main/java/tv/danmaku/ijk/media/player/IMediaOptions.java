package tv.danmaku.ijk.media.player;

public interface IMediaOptions
{
    void setOption(int category, String name, String value);
    void setOption(int category, String name, long value);
}
