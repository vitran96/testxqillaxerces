#include "testdomlsinput.h"

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
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <chrono>

#define TEST_FILE "sample2.xml"

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
    bool handleError(const DOMError& domError) override
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

    void resetErrors() {};
};

DOMDocument* ParseFile(const std::string& file);
void PrintDOMElements(const std::list<DOMElement*>& elementsList);

void PrintDOMNode(DOMNode* domNode);

DOMDocument* XQillaParseFile(const std::string& file);

DOMDocumentFragment* ParseFileIntoExistingDomDocument(const std::string& file, DOMDocument* document);
DOMDocumentFragment* ParseFileThanManuallyAddIntoExistingDomDocument(const std::string& file, DOMDocument* document);

DOMImplementation* GetDOMImplementation();

void PrintNodeType(const DOMNode::NodeType& nodeType);

int mainDOMLSInputTest(const int argc, const char* argv[]);

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

const bool PRINT_RESULT = false;

const short XPATH_CASE_1(1);
const short XPATH_CASE_2(2);
const short XPATH_CASE_3(3);
const short XPATH_CASE_4(4);

const short TEST_XPATH_CASE = XPATH_CASE_1;

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

    int result = ::mainDOMLSInputTest(argc, argv);

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
            break;
        }
        case DOMImplName::XQILLA:
        {
            std::cout << "Initialize XQILLA" << std::endl;
            XQillaPlatformUtils::initialize();
            break;
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
            break;
        }
        case DOMImplName::XQILLA:
        {
            std::cout << "Terminate XQILLA" << std::endl;
            XQillaPlatformUtils::terminate();
            break;
        }
    }
}

int mainDOMLSInputTest(const int argc, const char* argv[])
{
    int returnCode = 0;

    std::string xmlFile(TEST_FILE);

    DOMDocument* xercesDoc;

    try
    {
        long long startTime(GetTimestamp());

        xercesDoc = ::XQillaParseFile(xmlFile);

        std::cout << "Finish parsing" << std::endl;

        long long afterParsingAFile(GetTimestamp());

        auto fragment1 = ParseFileIntoExistingDomDocument(xmlFile, xercesDoc);

        std::cout << "Finish parsing" << std::endl;

        long long afterParsingAFileIntoExistingDocument(GetTimestamp());

        auto fragment2 = ParseFileThanManuallyAddIntoExistingDomDocument(xmlFile, xercesDoc);

        std::cout << "Finish parsing" << std::endl;

        long long afterParsingAFileIntoExistingDocumentManually(GetTimestamp());

        std::cout << "Parsing time: " << (afterParsingAFile - startTime) << std::endl;
        std::cout << "Parsing time into existing Document: " << (afterParsingAFileIntoExistingDocument - afterParsingAFile) << std::endl;
        std::cout << "Parsing time into existing Document MANUALLY: " << (afterParsingAFileIntoExistingDocumentManually - afterParsingAFileIntoExistingDocument) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "\n" << "Error: " << e.what() << std::endl;
        returnCode = 1;
    }
    catch (const DOMException& e)
    {
        std::cerr << "DOMException: " << UTF8(e.getMessage()) << std::endl;
        returnCode = 1;
    }
    catch (...)
    {
        std::cerr << "UNKNOWN error occurred!!" << std:: endl;
        returnCode = 1;
    }

    if (xercesDoc == nullptr)
        std::cout << "Fail to load doc!" << std::endl;
    else
    {
        std::cout << "Will delete document" << std::endl;
        xercesDoc->release();
    }

    return returnCode;
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

    LocalFileInputSource fileInputSource(X(file.c_str()));
    input->setByteStream(&fileInputSource);

    auto document = parser->parse(input);

    input->release();
    parser->release();

    return document;
}

DOMDocumentFragment* ParseFileIntoExistingDomDocument(const std::string& file, DOMDocument* document)
{
    auto fragment = document->createDocumentFragment();

    DOMImplementation* impl = ::GetDOMImplementation();
    DOMLSParser* parser = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, false);
    parser->getDomConfig()->setParameter(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    DOMLSInput* input = impl->createLSInput();

    LocalFileInputSource fileInputSource(X(file.c_str()));
    input->setByteStream(&fileInputSource);

    parser->parseWithContext(input, fragment, DOMLSParser::ACTION_APPEND_AS_CHILDREN);

    input->release();
    parser->release();

    return fragment;
}

DOMDocumentFragment* ParseFileThanManuallyAddIntoExistingDomDocument(const std::string& file, DOMDocument* document)
{
    auto fragment = document->createDocumentFragment();

    DOMImplementation* impl = ::GetDOMImplementation();
    DOMLSParser* parser = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    parser->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, false);
    //parser->getDomConfig()->setParameter(XMLUni::fgXercesUserAdoptsDOMDocument, true);

    DOMLSInput* input = impl->createLSInput();

    LocalFileInputSource fileInputSource(X(file.c_str()));
    input->setByteStream(&fileInputSource);

    auto tempDocument = parser->parse(input);

    auto child = tempDocument->getFirstChild();

    while (child != nullptr)
    {
        auto imported = document->importNode(child, true);
        fragment->appendChild(imported);

        child = child->getNextSibling();
    }

    input->release();
    parser->release();

    return fragment;
}

void PrintDOMElements(const std::list<DOMElement*>& elementsList)
{
    // DOMImpl
    DOMImplementation* domImpl = ::GetDOMImplementation();
    //-----------------------------------------------------

    // DOMLSOutput-----------------------------------------
    DOMLSOutput* theOutPut = domImpl->createLSOutput();
    theOutPut->setEncoding(X("UTF-8"));
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
    for (auto it = elementsList.begin(); it != elementsList.end(); it++)
        theSerializer->write(*it, theOutPut);
    //-----------------------------------------------------

    // Release memory--------------------------------------
    consoleOutputFormatTarget.flush();
    theOutPut->release();
    theSerializer->release();
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

void PrintNodeType(const DOMNode::NodeType& nodeType)
{
    switch (nodeType)
    {
        case DOMNode::ATTRIBUTE_NODE:
            std::cout << "ATTRIBUTE_NODE" << std::endl;
            break;
        case DOMNode::CDATA_SECTION_NODE:
            std::cout << "CDATA_SECTION_NODE" << std::endl;
            break;
        case DOMNode::COMMENT_NODE:
            std::cout << "COMMENT_NODE" << std::endl;
            break;
        case DOMNode::DOCUMENT_FRAGMENT_NODE:
            std::cout << "DOCUMENT_FRAGMENT_NODE" << std::endl;
            break;
        case DOMNode::DOCUMENT_NODE:
            std::cout << "DOCUMENT_NODE" << std::endl;
            break;
        case DOMNode::DOCUMENT_TYPE_NODE:
            std::cout << "DOCUMENT_TYPE_NODE" << std::endl;
            break;
        case DOMNode::ELEMENT_NODE:
            std::cout << "ELEMENT_NODE" << std::endl;
            break;
        case DOMNode::ENTITY_NODE:
            std::cout << "ENTITY_NODE" << std::endl;
            break;
        case DOMNode::ENTITY_REFERENCE_NODE:
            std::cout << "ENTITY_REFERENCE_NODE" << std::endl;
            break;
        case DOMNode::NOTATION_NODE:
            std::cout << "NOTATION_NODE" << std::endl;
            break;
        case DOMNode::PROCESSING_INSTRUCTION_NODE:
            std::cout << "PROCESSING_INSTRUCTION_NODE" << std::endl;
            break;
        case DOMNode::TEXT_NODE:
            std::cout << "TEXT_NODE" << std::endl;
            break;
    }
}
