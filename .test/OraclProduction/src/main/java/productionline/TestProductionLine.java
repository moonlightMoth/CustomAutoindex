package productionline;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class TestProductionLine {

    public static void main(String[] args)
    {
        List<Product> list = new ArrayList<>();

        Product product = new AudioPlayer("auName0", "spec0");

        list.add(product);

        product = new AudioPlayer("auName1", "spec1");
        list.add(product);
        product = new AudioPlayer("auName4", "spec4");
        list.add(product);
        product = new AudioPlayer("auName3", "spec3");
        list.add(product);
        product = new AudioPlayer("auName2", "spec2");
        list.add(product);


        Screen screen = new Screen("600x400",40,22);
        MonitorType monitorType = MonitorType.LED;

        product = new MoviePlayer("mvName4", screen, monitorType);
        list.add(product);
        product = new MoviePlayer("mvName3", screen, monitorType);
        list.add(product);
        product = new MoviePlayer("mvName2", screen, monitorType);
        list.add(product);
        product = new MoviePlayer("mvName1", screen, monitorType);
        list.add(product);
        product = new MoviePlayer("mvName0", screen, monitorType);
        list.add(product);


        print(list);
        System.out.println();
        System.out.println("***********************ONLY AUDIOPLAYERS***********************\n");
        printType(list, AudioPlayer.class);
        System.out.println();
        System.out.println("***********************ONLY MOVIEPLAYERS***********************\n");
        printType(list, MoviePlayer.class);
        System.out.println();

        EmployeeInfo employeeInfo = new EmployeeInfo();
        System.out.println(employeeInfo.toString());

        ProcessFiles processFiles = new ProcessFiles();

        try
        {
            processFiles.writeFile(employeeInfo);
            processFiles.writeFile(list);

        } catch (IOException e)
        {
            throw new RuntimeException(e);
        }


    }

    private static void print(List<Product> list)
    {
        System.out.println("**********************UNSORTED**********************\n");
        System.out.println(list+ "\n");
        Collections.sort(list);
        System.out.println("***********************SORTED***********************\n");
        System.out.println(list);
    }

    private static void printType(List<Product> list, Class<? extends Product> c)
    {
        for (Product product : list)
        {
            if (c.isAssignableFrom(product.getClass()))
                System.out.println(product + "\n");
        }
    }
}
