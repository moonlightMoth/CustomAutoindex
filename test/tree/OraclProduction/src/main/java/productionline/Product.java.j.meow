package productionline;

import java.util.Date;

public abstract class Product implements Item, Comparable<Product> {

    private int serialNumber;
    private Date manufacturedOn;
    private String name;
    private static int currentProductionNumber = 0;

    public Product(String name)
    {
        this.name = name;
        manufacturedOn = new Date();
        setProductionNumber(currentProductionNumber);
    }

    @Override
    public void setProductionNumber(int productionNumber)
    {
        serialNumber = currentProductionNumber;
        currentProductionNumber++;
    }

    public static int getCurrentProductionNumber()
    {
        return currentProductionNumber;
    }

    @Override
    public void setName(String name)
    {
        this.name = name;
    }

    @Override
    public String getName()
    {
        return name;
    }

    @Override
    public Date getManufactureDate()
    {
        return manufacturedOn;
    }

    @Override
    public int getSerialNumber()
    {
        return serialNumber;
    }

    @Override
    public int compareTo(Product o)
    {
        return name.compareTo(o.name);
    }

    @Override
    public String toString()
    {
        return  "\nManufacturer  : " + manufacturer + "\n" +
                "Serial Number : " + serialNumber + "\n" +
                "Date          : " + manufacturedOn + "\n" +
                "Name          : " + name;
    }
}
