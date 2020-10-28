#include "testxqilla.h"

#include <xercesc/dom/DOM.hpp>

#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>

#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

#include <xqilla/xqilla-dom3.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <list>

#include <chrono>

#define TEST_FILE "sample.xml"

long long GetTimestamp()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

class DOMPrintErrorHandler : public DOMErrorHandler
{
public:

    DOMPrintErrorHandler() {};
    ~DOMPrintErrorHandler() {};

    /** @name The error handler interface */
    bool handleError(const DOMError& domError) override;
    void resetErrors() {};

private:
    /* Unimplemented constructors and operators */
    //DOMPrintErrorHandler(const DOMErrorHandler &);
    //void operator=(const DOMErrorHandler &);

};

bool DOMPrintErrorHandler::handleError(const DOMError& domError)
{
    // Display whatever error message passed from the serializer
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        std::cerr << "\nWarning Message: ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        std::cerr << "\nError Message: ";
    else
        std::cerr << "\nFatal Message: ";

    char* msg = XMLString::transcode(domError.getMessage());
    std::cerr << msg << std::endl;
    XMLString::release(&msg);

    // Instructs the serializer to continue serialization if possible.
    return true;
}

DOMDocument* ParseFile(const std::string& file);
void PrintDOMElements(std::list<DOMElement*> elementsList);

void PrintDOMNode(DOMNode* domNode);

DOMDocument* XQillaParseFile(const std::string& file);

DOMImplementation* GetDOMImplementation();

int mainXpathTest(const int argc, const char* argv[]);

std::list<DOMElement*> GetElementByXpath(DOMDocument* document, const char* xpath);

void Initialize();
void Terminate();

enum class DOMImplName
{
    XERCESC,
    XQILLA
};

const DOMImplName CURRENT_IMPL_NAME(DOMImplName::XQILLA);

const XMLCh* DEFAULT_FEATURES = u"";
const XMLCh* XPATH_FEATURES = u"XPath2";

DOMImplementation* GetDOMImplementation()
{
    switch (CURRENT_IMPL_NAME)
    {
        case DOMImplName::XERCESC:
        {
            std::cout << "GET XercesC DOM Impl" << std::endl;
            return DOMImplementationRegistry::getDOMImplementation(DEFAULT_FEATURES);
        }
        case DOMImplName::XQILLA:
        {
            std::cout << "GET XQilla DOM Impl" << std::endl;
            return DOMImplementationRegistry::getDOMImplementation(XPATH_FEATURES);
        }
    }
}

int main(const int argc, const char* argv[])
{
    try
    {
        ::Initialize();
    }
    catch (const XMLException& eXerces)
    {
        std::cerr << "Error during Xerces-C initialisation.\n"
              << "Xerces exception message: "
              << UTF8(eXerces.getMessage()) << std::endl;
        return 1;
    }

    int result = ::mainXpathTest(argc, argv);

    ::Terminate();

    return result;
}

void Initialize()
{
    switch (CURRENT_IMPL_NAME)
    {
        case DOMImplName::XERCESC:
        {
            std::cout << "Initialize XERCESC" << std::endl;
            XMLPlatformUtils::Initialize();
        }
        case DOMImplName::XQILLA:
        {
            std::cout << "Initialize XQILLA" << std::endl;
            XQillaPlatformUtils::initialize();
        }
    }
}

void Terminate()
{
    switch (CURRENT_IMPL_NAME)
    {
        case DOMImplName::XERCESC:
        {
            std::cout << "Terminate XERCESC" << std::endl;
            XMLPlatformUtils::Terminate();
        }
        case DOMImplName::XQILLA:
        {
            std::cout << "Terminate XQILLA" << std::endl;
            XQillaPlatformUtils::terminate();
        }
    }
}

int mainXpathTest(const int argc, const char* argv[])
{
    std::cout << "This is a program to test Xalan 1.12.0 XPath 1.0 support.\n"
        << "'sample.xml' next to TestXercesXpathFeatures.exe will be used for testing.\n"
        << "If you want to use your XML file, rename and replace sample.xml.\n"
        << "This program only take 1 argument as XPath. Pass in more than 1 the program will only use the 1st one.\n";

    if (argc == 1)
    {
        std::cout << "Please pass in a XPath argument" << std::endl;
        return 1;
    }

    std::string xmlFile(TEST_FILE);
    std::string xpathExpression(argv[1]);

    std::cout << "\nXPath: " << xpathExpression << std::endl;

    try
    {
        long long startTime(GetTimestamp());

        auto xercesDoc = XQillaParseFile(xmlFile);

        long long afterParsingAFile(GetTimestamp());

        // std::cout << "Should be holding Document here" << std::endl;

        // long long afterAnXPathExpression(GetTimestamp());

        //PrintDOMElement(xercesElementsList);

        // xercesElementsList.clear();
        if (xercesDoc == nullptr)
            std::cout << "Fail to load doc!" << std::endl;
        else
        {
            std::cout << "Will delete document" << std::endl;
            // PrintDOMNode(xercesDoc);
            xercesDoc->release();
        }

        std::cout << "Parsing time: " << (afterParsingAFile - startTime) << std::endl;
        // std::cout << "XPath time: " << (afterAnXPathExpression - afterParsingAFile) << std::endl;

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << "\n" << e.what() << std::endl;
        return 1;
    }
    catch (const DOMException& e)
    {
        std::cerr << "DOMException: " << UTF8(e.getMessage()) << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "UNKNOWN error occurred!!" << std:: endl;
        return 1;
    }
}

DOMDocument* ParseFile(const std::string& file)
{
    XercesDOMParser parser;
    parser.setValidationScheme(XercesDOMParser::Val_Auto);
    parser.setDoNamespaces(true);
    parser.parse(file.c_str());

    return parser.adoptDocument();
}

DOMDocument* XQillaParseFile(const std::string& file)
{
    DOMImplementation* impl = ::GetDOMImplementation();
    DOMLSParser* parser = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, false);
    parser->getDomConfig()->setParameter(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    DOMLSInput* input = impl->createLSInput();
    input->setStringData(X(file.c_str()));

    // auto document = parser->parse(input);
    auto document = parser->parseURI(file.c_str());

    // std::cout << "Finish parsing" << std::endl;

    input->release();

    // std::cout << "Delete " << "Input" << " Done" << std::endl;

    parser->release();

    // std::cout << "Delete " << "parser" << " Done" << std::endl;

    return document;
}

void PrintDOMElements(std::list<DOMElement*> elementsList)
{
    std::cout << "\nFound " << elementsList.size() << " elements" << std::endl;

    // DOMImpl
    DOMImplementation* domImpl = ::GetDOMImplementation();
    //-----------------------------------------------------

    // DOMLSOutput-----------------------------------------
    DOMLSOutput* theOutPut = domImpl->createLSOutput();
    theOutPut->setEncoding(XMLString::transcode("UTF-8"));
    //-----------------------------------------------------

    // DOMLSSerializer-------------------------------------
    DOMLSSerializer* theSerializer = domImpl->createLSSerializer();
    //-----------------------------------------------------

    // Error Handler---------------------------------------
    DOMPrintErrorHandler myErrorHandler;
    //-----------------------------------------------------

    // Configure-------------------------------------------
    DOMConfiguration* serializerConfig = theSerializer->getDomConfig();
    //-----------------------------------------------------

    // Set Error Handler-----------------------------------
    serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, &myErrorHandler);
    //-----------------------------------------------------

    // Set Pretty Print------------------------------------
    if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    //-----------------------------------------------------

    // Format Target---------------------------------------
    StdOutFormatTarget consoleOutputFormatTarget;
    //-----------------------------------------------------

    //-----------------------------------------------------
    theOutPut->setByteStream(&consoleOutputFormatTarget);

    // Print-----------------------------------------------
    for (auto element : elementsList)
        theSerializer->write(element, theOutPut);
    //-----------------------------------------------------

    // Release memory--------------------------------------
    theOutPut->release();
    theSerializer->release();
    consoleOutputFormatTarget.flush();
    //-----------------------------------------------------

    std::cout << "\n";
}

void PrintDOMNode(DOMNode* node)
{
    // DOMImpl
    DOMImplementation* domImpl = ::GetDOMImplementation();
    //-----------------------------------------------------

    // DOMLSOutput-----------------------------------------
    DOMLSOutput* theOutPut = domImpl->createLSOutput();
    theOutPut->setEncoding(XMLString::transcode("UTF-8"));
    //-----------------------------------------------------

    // DOMLSSerializer-------------------------------------
    DOMLSSerializer* theSerializer = domImpl->createLSSerializer();
    //-----------------------------------------------------

    // Error Handler---------------------------------------
    DOMPrintErrorHandler myErrorHandler;
    //-----------------------------------------------------

    // Configure-------------------------------------------
    DOMConfiguration* serializerConfig = theSerializer->getDomConfig();
    //-----------------------------------------------------

    // Set Error Handler-----------------------------------
    serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, &myErrorHandler);
    //-----------------------------------------------------

    // Set Pretty Print------------------------------------
    if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    //-----------------------------------------------------

    // Format Target---------------------------------------
    StdOutFormatTarget consoleOutputFormatTarget;
    //-----------------------------------------------------

    //-----------------------------------------------------
    theOutPut->setByteStream(&consoleOutputFormatTarget);

    // Print-----------------------------------------------
    theSerializer->write(node, theOutPut);
    //-----------------------------------------------------

    // Release memory--------------------------------------
    theOutPut->release();
    theSerializer->release();
    consoleOutputFormatTarget.flush();
    //-----------------------------------------------------

    std::cout << "\n";
}

std::list<DOMElement*> GetElementByXpath(DOMDocument* document, const char* xpath)
{
    return std::list<DOMElement*>();
}