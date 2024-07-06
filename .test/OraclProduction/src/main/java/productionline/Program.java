package productionline;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.function.Predicate;

public class Program {

    private static List<Product> list = new ArrayList<>();
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args)
    {

        while (true)
        {
            System.out.println("------------------------------------------------------");
            System.out.println("Enter option: \n 1 - Create a product \n 2 - Display products \n 3 - Display statistics \n 4 - Stop program");
            String s = scanner.nextLine();
            int opt;

            try
            {
                opt = Integer.parseInt(s);
            }
            catch (NumberFormatException e)
            {
                System.out.println("Unsupported operation!");
                continue;
            }

            switch (opt)
            {
                case 1: runCreationScript(); break;
                case 2: displayProducts(); break;
                case 3: displayStats(); break;
                case 4: System.out.println("Bye!"); System.exit(0); break;
                default: System.out.println("Unsupported operation!");
            }

            System.out.println('\n');
        }
    }

    private static void displayStats()
    {
        System.out.println(
                "Total items produced          : " + list.size() + '\n' +
                "Number of AudioPlayers        : " + list.stream().filter(product -> product instanceof AudioPlayer).count() + '\n' +
                "Number of MoviePlayers        : " + list.stream().filter(product -> product instanceof MoviePlayer).count() + '\n');
    }

    private static void displayProducts()
    {
        System.out.println(list);
    }

    private static void runCreationScript()
    {
        System.out.println("Enter product type: \n 1 - AudioPlayer \n 2 - MoviePlayer");

        String s = scanner.nextLine();
        int opt;

        try
        {
            opt = Integer.parseInt(s);
            if (opt != 1 && opt != 2)
            {
                System.out.println("Invalid input!");
                return;
            }
        }
        catch (NumberFormatException e)
        {
            System.out.println("Invalid input!");
            return;
        }

        if (opt == 1)
        {
            System.out.println("Enter AudioPlayer name:");
            String name = scanner.nextLine();
            System.out.println("Enter AudioPlayer audio spec:");
            list.add(new AudioPlayer(name, scanner.nextLine()));
            return;
        }

        System.out.println("Enter MoviePlayer name:");
        String name = scanner.nextLine();

        System.out.println("Enter Screen resolution:");
        String res = scanner.nextLine();

        System.out.println("Enter Screen refresh rate (a positive number):");
        s = scanner.nextLine();
        int rr;

        try
        {
            rr = Integer.parseInt(s);
            if (rr < 1)
            {
                System.out.println("Invalid input!");
                return;
            }
        }
        catch (NumberFormatException e)
        {
            System.out.println("Invalid input!");
            return;
        }

        System.out.println("Enter Screen response time (a positive number):");
        s = scanner.nextLine();
        int rt;

        try
        {
            rt = Integer.parseInt(s);
            if (rt < 1)
            {
                System.out.println("Invalid input!");
                return;
            }
        }
        catch (NumberFormatException e)
        {
            System.out.println("Invalid input!");
            return;
        }

        System.out.println("Enter MonitorType (LED or LCD):");

        s = scanner.nextLine();

        if (!s.equals("LED") && !s.equals("LCD"))
        {
            System.out.println("Invalid input!");
            return;
        }

        MonitorType monitorType;

        if (s.equals("LCD"))
            monitorType = MonitorType.LCD;
        else
            monitorType = MonitorType.LED;

        Screen screen = new Screen(res, rr, rt);
        list.add(new MoviePlayer(name, screen, monitorType));
    }


}
