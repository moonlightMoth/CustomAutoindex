package productionline;

import java.io.*;
import java.nio.file.Path;
import java.util.List;

public class ProcessFiles {

    private Path p = Path.of("C:/LineTests");
    private Path p2 = Path.of("TestResults.txt");
    private Path p3 = Path.of(p.toString(), p2.toString());

    private void createDirectory()
    {
        Path path = Path.of(p.toString());
        path.toFile().mkdir();
    }

    public void writeFile(EmployeeInfo employeeInfo) throws IOException
    {
        writeFile(employeeInfo.toString());
    }

    public void writeFile(List<Product> products) throws IOException
    {
        writeFile(products.toString());
    }

    private void writeFile(String s) throws IOException
    {
        if (!Path.of(p.toString()).toFile().exists())
        {
            createDirectory();
        }

        FileWriter fw = new FileWriter(p3.toFile(),true);
        BufferedWriter bw = new BufferedWriter(fw);

        bw.append(s);
        bw.newLine();
        bw.flush();
        bw.close();
    }
}
