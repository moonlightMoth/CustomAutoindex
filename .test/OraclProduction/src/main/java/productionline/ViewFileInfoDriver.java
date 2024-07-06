package productionline;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class ViewFileInfoDriver {

    public static void main(String[] args)
    {
        try
        {
            FileReader fr = new FileReader("C:/LineTests/TestResults.txt");
            BufferedReader br = new BufferedReader(fr);

            String s = "";

            while ((s = br.readLine()) != null)
                System.out.println(s);

            br.close();
            fr.close();

        } catch (FileNotFoundException e)
        {
            System.out.println("FILE NOT FOUND!");
            throw new RuntimeException(e);
        } catch (IOException e)
        {
            System.out.println("CANNOT READ FILE!");
            throw new RuntimeException(e);
        }
    }
}
