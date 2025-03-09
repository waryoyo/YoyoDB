// YoyoDB.cpp : Defines the entry point for the application.
//

#include "YoyoDB.h"


using namespace nlohmann::literals;


int main()
{
	Database database = Database("yoyo.jsonl");

	// Create Test
	nlohmann::json o1 = R"( {"war": "yoyo"} )"_json;
	database.createDocument(o1);

	// Read Test
	std::cout << database.readDocument(1)["war"];
}
