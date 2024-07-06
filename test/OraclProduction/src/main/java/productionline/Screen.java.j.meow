package productionline;

public class Screen implements ScreenSpec{

    private String resolution;
    private int refreshRate;
    private int responseTime;

    public Screen(String resolution, int refreshRate, int responseTime)
    {
        this.resolution = resolution;
        this.refreshRate = refreshRate;
        this.responseTime = responseTime;
    }

    @Override
    public String getResolution()
    {
        return resolution;
    }

    @Override
    public int getRefreshRate()
    {
        return refreshRate;
    }

    @Override
    public int getResponseTime()
    {
        return responseTime;
    }

    @Override
    public String toString()
    {
        return  "Resolution    : " + resolution + "\n" +
                "Refresh Rate  : " + refreshRate + "\n" +
                "Response Time : " + responseTime;
    }
}
