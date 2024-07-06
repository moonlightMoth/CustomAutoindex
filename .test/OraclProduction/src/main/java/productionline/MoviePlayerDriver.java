package productionline;

public class MoviePlayerDriver {
    public static void main(String[] args)
    {
        Screen screen = new Screen("600x400",40,22);
        MonitorType monitorType = MonitorType.LED;

        MoviePlayer moviePlayer = new MoviePlayer("PlaterName", screen, monitorType);

        moviePlayer.play();
        moviePlayer.next();
        moviePlayer.previous();
        moviePlayer.stop();
        System.out.println(moviePlayer);;
    }
}
