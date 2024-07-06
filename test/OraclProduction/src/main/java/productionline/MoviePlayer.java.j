package productionline;


public class MoviePlayer extends Product implements MultimediaControl{

    private Screen screen;
    private MonitorType monitorType;
    private ItemType itemType = ItemType.VI;

    public MoviePlayer(String name, Screen screen, MonitorType monitorType)
    {
        super(name);
        this.screen = screen;
        this.monitorType = monitorType;
    }

    @Override
    public void play()
    {
        System.out.println("Playing");
    }

    @Override
    public void stop()
    {
        System.out.println("Stop");
    }

    @Override
    public void previous()
    {
        System.out.println("Previous");
    }

    @Override
    public void next()
    {
        System.out.println("Next");
    }

    @Override
    public String toString()
    {
        return
                super.toString() + "\n" +
                screen.toString() + "\n" +
                "MonitorType   : " + monitorType.toString() + "\n" +
                        "Type          : " + itemType.toString();
    }
}
