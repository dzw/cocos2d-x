#include "FileUtilsTest.h"

USING_NS_CC;

FileUtilsTests::FileUtilsTests()
{
    ADD_TEST_CASE(TestResolutionDirectories);
    ADD_TEST_CASE(TestSearchPath);
    ADD_TEST_CASE(TestFilenameLookup);
    ADD_TEST_CASE(TestIsFileExist);
    ADD_TEST_CASE(TestFileFuncs);
    ADD_TEST_CASE(TestDirectoryFuncs);
    ADD_TEST_CASE(TextWritePlist);
    ADD_TEST_CASE(TestWriteString);
    ADD_TEST_CASE(TestWriteData);
    ADD_TEST_CASE(TestWriteValueMap);
    ADD_TEST_CASE(TestWriteValueVector);
}

// TestResolutionDirectories

void TestResolutionDirectories::onEnter()
{
    FileUtilsDemo::onEnter();
    auto sharedFileUtils = FileUtils::getInstance();

    std::string ret;

    sharedFileUtils->purgeCachedEntries();
    _defaultSearchPathArray = sharedFileUtils->getSearchPaths();
    std::vector<std::string> searchPaths = _defaultSearchPathArray;
    searchPaths.insert(searchPaths.begin(),   "Misc");
    sharedFileUtils->setSearchPaths(searchPaths);

    _defaultResolutionsOrderArray = sharedFileUtils->getSearchResolutionsOrder();
    std::vector<std::string> resolutionsOrder = _defaultResolutionsOrderArray;

    resolutionsOrder.insert(resolutionsOrder.begin(), "resources-ipadhd");
    resolutionsOrder.insert(resolutionsOrder.begin()+1, "resources-ipad");
    resolutionsOrder.insert(resolutionsOrder.begin()+2, "resources-widehd");
    resolutionsOrder.insert(resolutionsOrder.begin()+3, "resources-wide");
    resolutionsOrder.insert(resolutionsOrder.begin()+4, "resources-hd");
    resolutionsOrder.insert(resolutionsOrder.begin()+5, "resources-iphone");

    sharedFileUtils->setSearchResolutionsOrder(resolutionsOrder);

    for( int i=1; i<7; i++) {
        auto filename = String::createWithFormat("test%d.txt", i);
        ret = sharedFileUtils->fullPathForFilename(filename->getCString());
        log("%s -> %s", filename->getCString(), ret.c_str());
    }
}

void TestResolutionDirectories::onExit()
{
    auto sharedFileUtils = FileUtils::getInstance();

    // reset search path
    sharedFileUtils->setSearchPaths(_defaultSearchPathArray);
    sharedFileUtils->setSearchResolutionsOrder(_defaultResolutionsOrderArray);
    FileUtilsDemo::onExit();
}

std::string TestResolutionDirectories::title() const
{
    return "FileUtils: resolutions in directories";
}

std::string TestResolutionDirectories::subtitle() const
{
    return "See the console";
}

// TestSearchPath

void TestSearchPath::onEnter()
{
    FileUtilsDemo::onEnter();
    auto sharedFileUtils = FileUtils::getInstance();

    std::string ret;

    sharedFileUtils->purgeCachedEntries();
    _defaultSearchPathArray = sharedFileUtils->getSearchPaths();
    std::vector<std::string> searchPaths = _defaultSearchPathArray;
    std::string writablePath = sharedFileUtils->getWritablePath();
    std::string fileName = writablePath+"external.txt";
    char szBuf[100] = "Hello Cocos2d-x!";
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (fp)
    {
        size_t ret = fwrite(szBuf, 1, strlen(szBuf), fp);
        CCASSERT(ret != 0, "fwrite function returned zero value");
        fclose(fp);
        if (ret != 0)
            log("Writing file to writable path succeed.");
    }

    searchPaths.insert(searchPaths.begin(), writablePath);
    searchPaths.insert(searchPaths.begin()+1,   "Misc/searchpath1");
    searchPaths.insert(searchPaths.begin()+2, "Misc/searchpath2");
    sharedFileUtils->setSearchPaths(searchPaths);

    _defaultResolutionsOrderArray = sharedFileUtils->getSearchResolutionsOrder();
    std::vector<std::string> resolutionsOrder = _defaultResolutionsOrderArray;

    resolutionsOrder.insert(resolutionsOrder.begin(), "resources-ipad");
    sharedFileUtils->setSearchResolutionsOrder(resolutionsOrder);

    for( int i=1; i<3; i++) {
        auto filename = String::createWithFormat("file%d.txt", i);
        ret = sharedFileUtils->fullPathForFilename(filename->getCString());
        log("%s -> %s", filename->getCString(), ret.c_str());
    }

    // Gets external.txt from writable path
    std::string fullPath = sharedFileUtils->fullPathForFilename("external.txt");
    log("external file path = %s", fullPath.c_str());
    if (fullPath.length() > 0)
    {
        fp = fopen(fullPath.c_str(), "rb");
        if (fp)
        {
            char szReadBuf[100] = {0};
            size_t read = fread(szReadBuf, 1, strlen(szBuf), fp);
            if (read > 0)
                log("The content of file from writable path: %s", szReadBuf);
            fclose(fp);
        }
    }
}

void TestSearchPath::onExit()
{
    FileUtils *sharedFileUtils = FileUtils::getInstance();

    // reset search path
    sharedFileUtils->setSearchPaths(_defaultSearchPathArray);
    sharedFileUtils->setSearchResolutionsOrder(_defaultResolutionsOrderArray);
    FileUtilsDemo::onExit();
}

std::string TestSearchPath::title() const
{
    return "FileUtils: search path";
}

std::string TestSearchPath::subtitle() const
{
    return "See the console";
}

// TestFilenameLookup

void TestFilenameLookup::onEnter()
{
    FileUtilsDemo::onEnter();

    auto sharedFileUtils = FileUtils::getInstance();

    ValueMap dict;
    dict["grossini.bmp"] = Value("Images/grossini.png");
    dict["grossini.xcf"] = Value("Images/grossini.png");

    sharedFileUtils->setFilenameLookupDictionary(dict);

    // Instead of loading carlitos.xcf, it will load grossini.png
    auto sprite = Sprite::create("grossini.xcf");
    this->addChild(sprite);

    auto s = Director::getInstance()->getWinSize();
    sprite->setPosition(s.width/2, s.height/2);
}

void TestFilenameLookup::onExit()
{

    FileUtils *sharedFileUtils = FileUtils::getInstance();

    // reset filename lookup
    sharedFileUtils->setFilenameLookupDictionary(ValueMap());

    FileUtilsDemo::onExit();
}

std::string TestFilenameLookup::title() const
{
    return "FileUtils: filename lookup";
}

// TestIsFileExist

void TestIsFileExist::onEnter()
{
    FileUtilsDemo::onEnter();
    auto s = Director::getInstance()->getWinSize();
    auto sharedFileUtils = FileUtils::getInstance();

    Label* label = nullptr;
    bool isExist = false;

    isExist = sharedFileUtils->isFileExist("Images/grossini.png");

    label = Label::createWithSystemFont(isExist ? "Images/grossini.png exists" : "Images/grossini.png doesn't exist", "", 20);
    label->setPosition(s.width/2, s.height/3);
    this->addChild(label);

    isExist = sharedFileUtils->isFileExist("Images/grossini.xcf");
    label = Label::createWithSystemFont(isExist ? "Images/grossini.xcf exists" : "Images/grossini.xcf doesn't exist", "", 20);
    label->setPosition(s.width/2, s.height/3*2);
    this->addChild(label);
}

void TestIsFileExist::onExit()
{

    FileUtils *sharedFileUtils = FileUtils::getInstance();

    // reset filename lookup
    sharedFileUtils->setFilenameLookupDictionary(ValueMap());

    FileUtilsDemo::onExit();
}

std::string TestIsFileExist::title() const
{
    return "FileUtils: check whether the file exists";
}

std::string TestIsFileExist::subtitle() const
{
    return "";
}

// TestFileFuncs

void TestFileFuncs::onEnter()
{
    FileUtilsDemo::onEnter();
    auto s = Director::getInstance()->getWinSize();
    auto sharedFileUtils = FileUtils::getInstance();

    int x = s.width/2,
        y = s.height/5;
    Label* label = nullptr;

    std::string filename = "__test.test";
    std::string filename2 = "__newtest.test";
    std::string filepath = sharedFileUtils->getWritablePath() + filename;
    std::string content = "Test string content to put into created file";
    std::string msg;

    FILE *out = fopen(filepath.c_str(), "w");
    fputs(content.c_str(), out);
    fclose(out);

    // Check whether file can be created
    if (sharedFileUtils->isFileExist(filepath))
    {
        label = Label::createWithSystemFont("Test file '__test.test' created", "", 20);
        label->setPosition(x, y * 4);
        this->addChild(label);

        // getFileSize Test
        long size = sharedFileUtils->getFileSize(filepath);
        msg = StringUtils::format("getFileSize: Test file size equals %ld", size);
        label = Label::createWithSystemFont(msg, "", 20);
        label->setPosition(x, y * 3);
        this->addChild(label);

        // renameFile Test
        if (sharedFileUtils->renameFile(sharedFileUtils->getWritablePath(), filename, filename2))
        {
            label = Label::createWithSystemFont("renameFile: Test file renamed to  '__newtest.test'", "", 20);
            label->setPosition(x, y * 2);
            this->addChild(label);

            // removeFile Test
            filepath = sharedFileUtils->getWritablePath() + filename2;
            if (sharedFileUtils->removeFile(filepath))
            {
                label = Label::createWithSystemFont("removeFile: Test file removed", "", 20);
                label->setPosition(x, y * 1);
                this->addChild(label);
            }
            else
            {
                label = Label::createWithSystemFont("removeFile: Failed to remove test file", "", 20);
                label->setPosition(x, y * 1);
                this->addChild(label);
            }
        }
        else
        {
            label = Label::createWithSystemFont("renameFile: Failed to rename test file to  '__newtest.test', further test skipped", "", 20);
            label->setPosition(x, y * 2);
            this->addChild(label);
        }
    }
    else
    {
        label = Label::createWithSystemFont("Test file can not be created, test skipped", "", 20);
        label->setPosition(x, y * 4);
        this->addChild(label);
    }
}

std::string TestFileFuncs::title() const
{
    return "FileUtils: file control functions";
}

std::string TestFileFuncs::subtitle() const
{
    return "";
}

// TestDirectoryFuncs

void TestDirectoryFuncs::onEnter()
{
    FileUtilsDemo::onEnter();
    auto s = Director::getInstance()->getWinSize();
    auto sharedFileUtils = FileUtils::getInstance();

    int x = s.width/2,
    y = s.height/4;
    Label* label = nullptr;

    std::string dir = sharedFileUtils->getWritablePath() + "__test/";
    std::string subDir = "dir1/dir2";
    std::string msg;
    bool ok;

    // Check whether dir can be created
    ok = sharedFileUtils->createDirectory(dir);
    if (ok && sharedFileUtils->isDirectoryExist(dir))
    {
        msg = StringUtils::format("createDirectory: Directory '__test' created");
        label = Label::createWithSystemFont(msg, "", 20);
        label->setPosition(x, y * 3);
        this->addChild(label);

        // Create sub directories recursively
        ok = sharedFileUtils->createDirectory(dir + subDir);
        if (ok && sharedFileUtils->isDirectoryExist(dir + subDir))
        {
            msg = StringUtils::format("createDirectory: Sub directories '%s' created", subDir.c_str());
            label = Label::createWithSystemFont(msg, "", 20);
            label->setPosition(x, y * 2);
            this->addChild(label);
        }
        else
        {
            msg = StringUtils::format("createDirectory: Failed to create sub directories '%s'", subDir.c_str());
            label = Label::createWithSystemFont(msg, "", 20);
            label->setPosition(x, y * 2);
            this->addChild(label);
        }

        // Remove directory
        ok = sharedFileUtils->removeDirectory(dir);
        if (ok && !sharedFileUtils->isDirectoryExist(dir))
        {
            msg = StringUtils::format("removeDirectory: Directory '__test' removed");
            label = Label::createWithSystemFont(msg, "", 20);
            label->setPosition(x, y);
            this->addChild(label);
        }
        else
        {
            msg = StringUtils::format("removeDirectory: Failed to remove directory '__test'");
            label = Label::createWithSystemFont(msg, "", 20);
            label->setPosition(x, y);
            this->addChild(label);
        }
    }
    else
    {
        msg = StringUtils::format("createDirectory: Directory '__test' can not be created");
        label = Label::createWithSystemFont(msg, "", 20);
        label->setPosition(x, y * 2);
        this->addChild(label);
    }
}

std::string TestDirectoryFuncs::title() const
{
    return "FileUtils: directory control functions";
}

std::string TestDirectoryFuncs::subtitle() const
{
    return "";
}

// TextWritePlist

void TextWritePlist::onEnter()
{
    FileUtilsDemo::onEnter();
    auto root = Dictionary::create();
    auto string = String::create("string element value");
    root->setObject(string, "string element key");

    auto array = Array::create();

    auto dictInArray = Dictionary::create();
    dictInArray->setObject(String::create("string in dictInArray value 0"), "string in dictInArray key 0");
    dictInArray->setObject(String::create("string in dictInArray value 1"), "string in dictInArray key 1");
    array->addObject(dictInArray);

    array->addObject(String::create("string in array"));

    auto arrayInArray = Array::create();
    arrayInArray->addObject(String::create("string 0 in arrayInArray"));
    arrayInArray->addObject(String::create("string 1 in arrayInArray"));
    array->addObject(arrayInArray);

    root->setObject(array, "array");

    auto dictInDict = Dictionary::create();
    dictInDict->setObject(String::create("string in dictInDict value"), "string in dictInDict key");

    //add boolean to the plist
    auto booleanObject = Bool::create(true);
    dictInDict->setObject(booleanObject, "bool");

    //add interger to the plist
    auto intObject = Integer::create(1024);
    dictInDict->setObject(intObject, "integer");

    //add float to the plist
    auto floatObject = Float::create(1024.1024f);
    dictInDict->setObject(floatObject, "float");

    //add double to the plist
    auto doubleObject = Double::create(1024.123);
    dictInDict->setObject(doubleObject, "double");



    root->setObject(dictInDict, "dictInDict, Hello World");

    // end with /
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + "text.plist";
    if(root->writeToFile(fullPath.c_str()))
        log("see the plist file at %s", fullPath.c_str());
    else
        log("write plist file failed");

    auto label = Label::createWithTTF(fullPath.c_str(), "fonts/Thonburi.ttf", 6);
    this->addChild(label);
    auto winSize = Director::getInstance()->getWinSize();
    label->setPosition(winSize.width/2, winSize.height/3);

    auto loadDict = __Dictionary::createWithContentsOfFile(fullPath.c_str());
    auto loadDictInDict = (__Dictionary*)loadDict->objectForKey("dictInDict, Hello World");
    auto boolValue = (__String*)loadDictInDict->objectForKey("bool");
    log("%s",boolValue->getCString());
    auto floatValue = (__String*)loadDictInDict->objectForKey("float");
    log("%s",floatValue->getCString());
    auto intValue = (__String*)loadDictInDict->objectForKey("integer");
    log("%s",intValue->getCString());
    auto doubleValue = (__String*)loadDictInDict->objectForKey("double");
    log("%s",doubleValue->getCString());

}

void TextWritePlist::onExit()
{
    FileUtilsDemo::onExit();
}

std::string TextWritePlist::title() const
{
    return "FileUtils: Dictionary to plist";
}

std::string TextWritePlist::subtitle() const
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath().c_str();
    return ("See plist file at your writablePath");
}

void TestWriteString::onEnter()
{
    FileUtilsDemo::onEnter();

    auto winSize = Director::getInstance()->getWinSize();

    auto writeResult = Label::createWithTTF("show writeResult", "fonts/Thonburi.ttf", 18);
    this->addChild(writeResult);
    writeResult->setPosition(winSize.width / 2, winSize.height * 3 / 4);

    auto readResult = Label::createWithTTF("show readResult", "fonts/Thonburi.ttf", 18);
    this->addChild(readResult);
    readResult->setPosition(winSize.width / 2, winSize.height / 3);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fileName = "writeStringTest.txt";

    // writeTest
    std::string writeDataStr = "the string data will be write into a file";
    std::string fullPath = writablePath + fileName;
    if (FileUtils::getInstance()->writeStringToFile(writeDataStr, fullPath.c_str()))
    {
        log("see the plist file at %s", fullPath.c_str());
        writeResult->setString("write success:" + writeDataStr);
    }
    else
    {
        log("write plist file failed");
        writeResult->setString("write fail");
    }

    // readTest
    std::string readDataStr = FileUtils::getInstance()->getStringFromFile(fullPath);
    readResult->setString("read success:" + readDataStr);
}

void TestWriteString::onExit()
{
    FileUtilsDemo::onExit();
}

std::string TestWriteString::title() const
{
    return "FileUtils: TestWriteString to files";
}

std::string TestWriteString::subtitle() const
{
    return "";
}

void TestWriteData::onEnter()
{
    FileUtilsDemo::onEnter();

    auto winSize = Director::getInstance()->getWinSize();

    auto writeResult = Label::createWithTTF("show writeResult", "fonts/Thonburi.ttf", 18);
    this->addChild(writeResult);
    writeResult->setPosition(winSize.width / 2, winSize.height * 3 / 4);

    auto readResult = Label::createWithTTF("show readResult", "fonts/Thonburi.ttf", 18);
    this->addChild(readResult);
    readResult->setPosition(winSize.width / 2, winSize.height / 3);

    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fileName = "writeDataTest.txt";

    // writeTest
    std::string writeDataStr = "the binary data will be write into a file";
    Data writeData;
    writeData.copy((unsigned char *)writeDataStr.c_str(), writeDataStr.size());
    std::string fullPath = writablePath + fileName;
    if (FileUtils::getInstance()->writeDataToFile(writeData, fullPath.c_str()))
    {
        log("see the plist file at %s", fullPath.c_str());
        writeResult->setString("write success:" + writeDataStr);
    }
    else
    {
        log("write plist file failed");
        writeResult->setString("write fail");
    }

    // readTest
    unsigned char* buffer = nullptr;
    Data readData = FileUtils::getInstance()->getDataFromFile(fullPath);
    buffer = (unsigned char*)malloc(sizeof(unsigned char) * (readData.getSize() + 1));
    memcpy(buffer, readData.getBytes(), readData.getSize());
    buffer[readData.getSize()] = '\0';
    std::string readDataStr((const char*)buffer);
    free(buffer);

    readResult->setString("read success:" + readDataStr);
}

void TestWriteData::onExit()
{
    FileUtilsDemo::onExit();
}

std::string TestWriteData::title() const
{
    return "FileUtils: TestWriteData to files";
}

std::string TestWriteData::subtitle() const
{
    return "";
}

void TestWriteValueMap::onEnter()
{
    FileUtilsDemo::onEnter();

    auto winSize = Director::getInstance()->getWinSize();

    auto writeResult = Label::createWithTTF("show writeResult", "fonts/Thonburi.ttf", 18);
    this->addChild(writeResult);
    writeResult->setPosition(winSize.width / 2, winSize.height * 3 / 4);

    auto readResult = Label::createWithTTF("show readResult", "fonts/Thonburi.ttf", 18);
    this->addChild(readResult);
    readResult->setPosition(winSize.width / 2, winSize.height / 3);

    ValueMap valueMap;

    ValueMap mapInValueMap;
    mapInValueMap["string1"] = "string in dictInMap key 0";
    mapInValueMap["string2"] = "string in dictInMap key 1";
    valueMap["data0"] = Value(mapInValueMap);

    valueMap["data1"] = Value("string in array");

    ValueVector arrayInMap;
    arrayInMap.push_back(Value("string 0 in arrayInMap"));
    arrayInMap.push_back(Value("string 1 in arrayInMap"));
    valueMap["data2"] = arrayInMap;

    //add boolean to the plist
    auto booleanObject = Value(true);
    valueMap["data3"] = booleanObject;

    //add interger to the plist
    auto intObject = Value(1024);
    valueMap["data4"] = intObject;

    //add float to the plist
    auto floatObject = Value(1024.1024f);
    valueMap["data5"] = floatObject;

    //add double to the plist
    auto doubleObject = Value(1024.123);
    valueMap["data6"] = doubleObject;


    // end with /
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + "testWriteValueMap.plist";
    if (FileUtils::getInstance()->writeValueMapToFile(valueMap, fullPath.c_str()))
    {
        log("see the plist file at %s", fullPath.c_str());
        writeResult->setString("write success");
    }
    else
    {
        log("write plist file failed");
        writeResult->setString("write failed");
    }

    ValueMap readValueMap = FileUtils::getInstance()->getValueMapFromFile(fullPath.c_str());
    std::string readDataStr = "read data:\n";
    // read value map data
    ValueMap readMapInMap = readValueMap["data0"].asValueMap();
    readDataStr += "  mapValue:[\"string1\"][" + readMapInMap["string1"].asString() + "]\n";
    readDataStr += "  mapValue:[\"string2\"][" + readMapInMap["string2"].asString() + "]\n";

    // read string data
    readDataStr += "  stringValue:" + readValueMap["data1"].asString() + "\n";

    // read value vector data
    ValueVector readVectorInMap = readValueMap["data2"].asValueVector();
    readDataStr += "  vectorValue:[1]" + readVectorInMap.at(0).asString() + "\n";
    readDataStr += "  vectorValue:[2]" + readVectorInMap.at(1).asString() + "\n";

    // read bool data
    readDataStr += "  boolValue:" + StringUtils::format("%d", readValueMap["data3"].asBool()) + "\n";

    // read int data
    readDataStr += "  intValue:" + StringUtils::format("%d", readValueMap["data4"].asInt()) + "\n";

    // read float data
    readDataStr += "  floatValue:" + StringUtils::format("%f", readValueMap["data5"].asFloat()) + "\n";

    // read double data
    readDataStr += "  doubleValue:" + StringUtils::format("%f", readValueMap["data6"].asDouble()) + "\n";

    readResult->setString(readDataStr);
}
void TestWriteValueMap::onExit()
{
    FileUtilsDemo::onExit();
}

std::string TestWriteValueMap::title() const
{
    return "FileUtils: TestWriteValueMap to files";
}

std::string TestWriteValueMap::subtitle() const
{
    return "";
}

void TestWriteValueVector::onEnter()
{
    FileUtilsDemo::onEnter();

    auto winSize = Director::getInstance()->getWinSize();

    auto writeResult = Label::createWithTTF("show writeResult", "fonts/Thonburi.ttf", 18);
    this->addChild(writeResult);
    writeResult->setPosition(winSize.width / 2, winSize.height * 3 / 4);

    auto readResult = Label::createWithTTF("show readResult", "fonts/Thonburi.ttf", 18);
    this->addChild(readResult);
    readResult->setPosition(winSize.width / 2, winSize.height / 3);

    ValueVector array;

    ValueMap mapInArray;
    mapInArray["string1"] = "string in dictInArray key 0";
    mapInArray["string2"] = "string in dictInArray key 1";
    array.push_back(Value(mapInArray));

    array.push_back(Value("string in array"));

    ValueVector arrayInArray;
    arrayInArray.push_back(Value("string 0 in arrayInArray"));
    arrayInArray.push_back(Value("string 1 in arrayInArray"));
    array.push_back(Value(arrayInArray));

    //add boolean to the plist
    auto booleanObject = Value(true);
    array.push_back(booleanObject);

    //add interger to the plist
    auto intObject = Value(1024);
    array.push_back(intObject);

    //add float to the plist
    auto floatObject = Value(1024.1024f);
    array.push_back(floatObject);

    //add double to the plist
    auto doubleObject = Value(1024.123);
    array.push_back(doubleObject);


    // end with /
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + "testWriteValueVector.plist";
    if (FileUtils::getInstance()->writeValueVectorToFile(array, fullPath.c_str()))
    {
        log("see the plist file at %s", fullPath.c_str());
        writeResult->setString("write success");
    }
    else
    {
        log("write plist file failed");
        writeResult->setString("write failed");
    }

    ValueVector readArray = FileUtils::getInstance()->getValueVectorFromFile(fullPath.c_str());
    std::string readDataStr = "read data:\n";
    // read value map data
    ValueMap readMapInArray = readArray.at(0).asValueMap();
    readDataStr += "  mapValue:[\"string1\"][" + readMapInArray["string1"].asString() + "]\n";
    readDataStr += "  mapValue:[\"string2\"][" + readMapInArray["string2"].asString() + "]\n";

    // read string data
    readDataStr += "  stringValue:" + readArray.at(1).asString() + "\n";

    // read value vector data
    ValueVector readVectorInArray = readArray.at(2).asValueVector();
    readDataStr += "  vectorValue:[1]" + readVectorInArray.at(0).asString() + "\n";
    readDataStr += "  vectorValue:[2]" + readVectorInArray.at(1).asString() + "\n";

    // read bool data
    readDataStr += "  boolValue:" + StringUtils::format("%d", readArray.at(3).asBool()) + "\n";

    // read int data
    readDataStr += "  intValue:" + StringUtils::format("%d", readArray.at(4).asInt()) + "\n";

    // read float data
    readDataStr += "  floatValue:" + StringUtils::format("%f", readArray.at(5).asFloat()) + "\n";

    // read double data
    readDataStr += "  doubleValue:" + StringUtils::format("%f", readArray.at(6).asDouble()) + "\n";

    readResult->setString(readDataStr);
}

void TestWriteValueVector::onExit()
{
    FileUtilsDemo::onExit();
}

std::string TestWriteValueVector::title() const
{
    return "FileUtils: TestWriteValueVector to files";
}

std::string TestWriteValueVector::subtitle() const
{
    return "";
}
