
import java.util.*;
import djondb.*;
import java.util.logging.Level;
import java.util.logging.Logger;

class Main {

    public static void testBSON() {
        System.out.println("testing testBSON");
        BSONObj wrapper = new BSONObj();
        wrapper.add("i", 1);
        wrapper.add("d", 100120d);
        wrapper.add("l", 12l);
        wrapper.add("s", "Test");
        BSONObj inner = new BSONObj();
        inner.add("s", "s");
        wrapper.add("inner", inner);
        assert (5 == wrapper.length());
        assert (wrapper.has("i"));
        assert (wrapper.has("d"));
        assert (wrapper.has("l"));
        assert (wrapper.has("s"));
        assert (1 == wrapper.getInt("i"));
        assert (100120d == wrapper.getDouble("d"));
        assert (12l == wrapper.getLong("l"));
        assert ("Test".equals(wrapper.getString("s")));
        BSONObj innerres = wrapper.getBSON("inner");
        assert (innerres != null);
        assert ("Test".equals(innerres.getString("s")));

        // test arrays
        BSONArrayObj array = new BSONArrayObj();
        System.out.println("Array");
        BSONObj obj = new BSONObj();
        obj.add("i", 1);
        System.out.println("adding to Array");
        array.add(obj);
        wrapper.add("array", array);
        assert (wrapper.getBSONArray("array") != null);
        assert (wrapper.getBSONArray("array").length() == 1);
        //	public BSONArrayObj* getBSONArray(String key) ;
        //
        assert (wrapper.get("s") != null);
        //wrapper.getXpath(String xpath);
        //wrapper.toString();
        //wrapper.iterator();
    }

    public void testConnection() {
        System.out.println("testing testConnection");
        try {
            Connection con = ConnectionManager.getConnection("localhost");

            if (!con.open()) {
                System.out.println("Not connected");
            }

            con.insert("dbjava", "nsjava", "{ 'name': 'Juan' }");
            String obj = "{ \"name\": \"John\", \"lastName\": \"Doe\"}";
            con.insert("testdb", "javans", obj);
            BSONObjVectorPtr result = con.find("dbjava", "nsjava", "$'name' == 'Juan'");

            System.out.println("Size: " + result.size());

            for (int x = 0; x < result.size(); x++) {
                BSONObj bson = result.get(x);
                System.out.println(bson.toString());
            }

            ConnectionManager.releaseConnection(con);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void testFind() {
        System.out.println("testing testFind");
        try {
            Connection con = ConnectionManager.getConnection("localhost");

            if (!con.open()) {
                System.out.println("Not connected");
            }

            con.dropNamespace("dbjava", "nsjava");
            
            con.insert("dbjava", "nsjava", "{ 'name': 'Juan' }");
            String obj = "{ \"name\": \"John\", \"lastName\": \"Doe\", \"inner\": { \"innerval\": 1}, \"array\": [1, 2, 3]}";
            con.insert("testdb", "javans", obj);
            BSONObjVectorPtr wrapper = con.find("testdb", "javans", "");

            System.out.println("Size: " + wrapper.size());
            for (int x = 0; x < wrapper.size(); x++) {
                BSONObj bson = wrapper.get(x);
		BSONObj inner = bson.getBSON("inner");
                System.out.println(inner.getInt("innerval"));
            }

            ConnectionManager.releaseConnection(con);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        try {
            Loader.initialize();
            Main m = new Main();
            m.testBSON();
            m.testConnection();
            m.testFind();
        } catch (LibraryException ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
