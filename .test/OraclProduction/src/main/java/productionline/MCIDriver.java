package productionline;

public class MCIDriver {

    public static void main(String[] args)
    {
        MultimediaControl mmc = new AudioPlayer("NAME", "audioPrikol");

        mmc.play();
        mmc.next();
        mmc.previous();
        mmc.stop();
        System.out.println(mmc);

        Screen screen = new Screen("600x400",40,22);
        MonitorType monitorType = MonitorType.LED;

        mmc = new MoviePlayer("PlaterName", screen, monitorType);

        mmc.play();
        mmc.next();
        mmc.previous();
        mmc.stop();
        System.out.println(mmc);


    }
}
