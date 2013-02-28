package djondb;

import djondb.*;
import java.util.ArrayList;

public class HelloWorld {

    public static void main(String[] args) {
        HelloWorld test = new HelloWorld();
        test.helloWorld();
    }

    public void helloWorld() {
        try {
            Loader.initialize();

            Connection con = ConnectionManager.getConnection("localhost");

            if (!con.open()) {
                System.out.println("Not connected");
                System.exit(0);
            }

            con.insert("dbjava", "nsjava", "{ 'name': 'John', 'lastName': 'Smith' }");

            BSONObjVectorPtr wrapper = con.find("dbjava", "nsjava", "$'lastName' == 'Smith'");

            BSONObj bson = wrapper.get(0);

            System.out.println("Hello: " + bson.getString("name"));

            ConnectionManager.releaseConnection(con);
        } catch (LibraryException e) {
            e.printStackTrace();
        }
    }
}
