// YoyoDB.cpp : Defines the entry point for the application.
//

#include "YoyoDB.h"


using namespace nlohmann::literals;


int main()
{
	Database database = Database("yoyo");
	database.createFieldIndex("war", true);
	QueryParser parser = QueryParser();
	auto tree = parser.parse(R"( {"field": "war", "operator": "eq", "value": "yoyo3"} )"_json);
	QueryExecutor executor = QueryExecutor();
	auto result = executor.execute(tree.get(), database);

	for (const auto& item : result) {
		std::cout << item.dump() << '\n';
	}

	// Create Test
	/*nlohmann::json o1 = R"( {"war": "yoyo"} )"_json;
	nlohmann::json o2 = R"( {"war": "yoyo2"} )"_json;
	nlohmann::json o3 = R"( {"war": "yoyo3"} )"_json;
	nlohmann::json o4 = R"( {"war": "yoyo4"} )"_json;*/

	//database.createDocument(o1);
	//database.createDocument(o2);
	//database.createDocument(o3);
	//database.createDocument(o4);

	/*auto items = database.readAllDocuments();

	for (const auto& item : items) {
		std::cout << item.dump() << '\n';
	}*/

	//// Read Test
	////std::cout << database.readDocument(1)["war"] << '\n';

	////// Update Test
	/*nlohmann::json new_item = R"( {"mega": "wowo"} )"_json;
	database.updateDocument(2, new_item);
	std::cout << database.readDocument(2)["mega"] << '\n';*/


	//// Delete Test
	//database.deleteDocument(3);
	//std::cout << database.readDocument(3);

}
