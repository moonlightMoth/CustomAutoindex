package productionline;

import java.util.Scanner;
import java.util.regex.Pattern;

public class EmployeeInfo {
    private StringBuilder name;
    private String code;
    private String dptId;
    private Pattern p = Pattern.compile("[A-Z][a-z]{3}[0-9]{2}");
    private Scanner in = new Scanner(System.in);;

    public EmployeeInfo()
    {
        String s = inputName();
        setDptId();
        StringBuilder sb = new StringBuilder(s);
        setName(sb);
        createEmployeeCode(sb);


    }

    public String getDptId()
    {
        System.out.println("Enter DptId:");
        return in.nextLine();
    }

    private void setDptId()
    {
        String id = getDptId();
        if (validId(id))
        {
            id = reverseString(id);
            this.dptId = id;
        }
        else
        {
            this.dptId = "None01";
        }
    }

    private String getId()
    {
        return dptId;
    }

    private boolean validId(String id)
    {
        return p.matcher(id).matches();
    }

    public StringBuilder getName()
    {
        return name;
    }

    private void setName(StringBuilder name)
    {
        this.name = name;
    }

    public String getCode()
    {
        return code;
    }

    private String inputName()
    {
        System.out.println("Enter name:");
        return in.nextLine();
    }

    private boolean checkName(StringBuilder name)
    {
        int cnt = 0;
        for (int i = 0; i < name.length(); i++)
        {
            if (name.charAt(i) == ' ')
                cnt++;
        }

        return cnt == 1 && name.indexOf(" ") != 0 && name.indexOf(" ") != name.length()-1;
    }

    private void createEmployeeCode(StringBuilder name)
    {
        if (checkName(name))
        {
            char c = name.charAt(0);
            name.replace(0, name.indexOf(" "), "");
            name.delete(0,1);
            name.append(c);
            code = name.toString();
        }
        else
        {
            code = "GUEST";
        }
    }

    public String reverseString(String id)
    {
        return new StringBuilder(id).reverse().toString();
    }

    @Override
    public String toString()
    {
        return
                        "\nCode          : " + code + '\n' +
                        "Department ID : " + dptId + '\n';
    }
}
