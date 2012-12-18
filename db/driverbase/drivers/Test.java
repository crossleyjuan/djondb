import djondb.*;

class Test {
	public static void main(String[] arg) {
		try {
			Loader.initialize();
			DjondbConnection c = DjondbConnectionManager.getConnection("localhost");

			c.open();

			String hero = "{ name: 'Peter', lastName: 'Parker', occupations: [ { company: 'Daily Bugle', position: 'Photographer'}, { position: 'Superhero' } ], nicknames: [{ name: 'Spiderman', main: 1}, {'name': 'Spìdey'}] }";

			c.insert("javadb", "superheroes", hero);

			BSONObjVectorPtr res = c.find("javadb", "superheroes", "$'name' == 'Peter'");

			for  (int x = 0; x < res.size(); x++) {
				BSONObj bhero = res.get(x);
				System.out.println("Hero name: " + bhero.getString("name"));
			}

			DjondbConnectionManager.releaseConnection(c);
		} catch (LibraryException e) {
			e.printStackTrace();
		}
	}
}
