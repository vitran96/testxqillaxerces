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

DOMImplementation* GetDOMImplementation();

void PrintNodeType(const DOMNode::NodeType& nodeType);

int mainXpathTest(const int argc, const char* argv[]);

std::list<DOMElement*> GetElementByXpath(DOMDocument* document, const std::string& xpath);

DOMElement* DetachRootElement(DOMDocument* document);
DOMDocumentFragment* DetachRootAndAddToDocumentFragment(DOMDocument* document);

std::list<DOMElement*> GetElementByXpathFromDetachedElement(DOMDocument* document, DOMElement* element, const std::string& xpath);
std::list<DOMElement*> GetElementByXpathFromDocumentFragment(DOMDocument* document, DOMDocumentFragment* docFragment, const std::string& xpath);


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

int mainXpathTest(const int argc, const char* argv[])
{
    std::cout << "This is a program to test XQilla 2.3.4 XPath 2.0 support.\n"
        << "'sample.xml' next to TestXqilla will be used for testing.\n"
        << "If you want to use your XML file, rename and replace sample.xml.\n"
        << "This program only take 1 argument as XPath. Pass in more than 1 the program will only use the 1st one.\n";

    if (argc == 1)
    {
        std::cout << "Please pass in a XPath argument" << std::endl;
        return 1;
    }

    int returnCode = 0;

    std::string xmlFile(TEST_FILE);
    std::string xpathExpression(argv[1]);

    std::cout << "\nXPath: " << xpathExpression << std::endl;

    DOMDocument* xercesDoc;

    try
    {
        std::list<DOMElement*> xercesElementsList;

        long long startTime(GetTimestamp());

        xercesDoc = ::XQillaParseFile(xmlFile);

        std::cout << "Finish parsing" << std::endl;

        long long afterParsingAFile(GetTimestamp());

        if (TEST_XPATH_CASE == XPATH_CASE_1)
            xercesElementsList = ::GetElementByXpath(xercesDoc, xpathExpression);
        else if (TEST_XPATH_CASE == XPATH_CASE_2)
        {
            DOMElement* root = ::DetachRootElement(xercesDoc);
            xercesElementsList = ::GetElementByXpathFromDetachedElement(xercesDoc, root, xpathExpression);
        }
        else if (TEST_XPATH_CASE == XPATH_CASE_3)
        {
            DOMElement* root = ::DetachRootElement(xercesDoc);
            DOMDocumentFragment* docFragment = xercesDoc->createDocumentFragment();
            docFragment->appendChild(root);
            xercesElementsList = ::GetElementByXpathFromDetachedElement(xercesDoc, root, xpathExpression);
        }
        else if (TEST_XPATH_CASE == XPATH_CASE_4)
        {
            DOMDocumentFragment* docFragment = ::DetachRootAndAddToDocumentFragment(xercesDoc);
            xercesElementsList = ::GetElementByXpathFromDocumentFragment(xercesDoc, docFragment, xpathExpression);
        }

        long long afterAnXPathExpression(GetTimestamp());

        std::cout << "Finish XPath resolving" << std::endl;
        std::cout << "Found " << xercesElementsList.size() << " elements" << std::endl;


        if (PRINT_RESULT)
        {
            ::PrintDOMElements(xercesElementsList);
            std::cout << "Finish print xpath results" << std::endl;
        }

        xercesElementsList.clear();

        std::cout << "Parsing time: " << (afterParsingAFile - startTime) << std::endl;
        std::cout << "XPath time: " << (afterAnXPathExpression - afterParsingAFile) << std::endl;
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

std::list<DOMElement*> GetElementByXpath(DOMDocument* document, const std::string& xpath)
{
    try
    {
        std::list<DOMElement*> resultList;

        // TODO: release manually

        AutoRelease<DOMXPathNSResolver> resolver(document->createNSResolver(document->getDocumentElement()));
        AutoRelease<DOMXPathExpression> parsedExpression(document->createExpression(X(xpath.c_str()), resolver));

        AutoRelease<DOMXPathResult> result(
            parsedExpression->evaluate(
                document->getDocumentElement(),
                DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
                nullptr
            )
        );

        size_t nLength = result->getSnapshotLength();

        if (nLength == 0)
            throw std::runtime_error("No result");

        for (size_t i = 0; i < nLength; i++)
        {
            result->snapshotItem(i);

            auto tempNode = result->getNodeValue();

            if (tempNode->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                PrintNodeType(tempNode->getNodeType());
                throw std::runtime_error("Result contain non-element node");
            }

            resultList.push_back(dynamic_cast<DOMElement*>(tempNode));
        }

        return resultList;
    }
    catch (const XQillaException& ex)
    {
        std::cout << "\n" << "XQillaException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMXPathException& ex)
    {
        std::cout << "\n" << "DOMXPathException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMException& ex)
    {
        std::cout << "\n" << "DOMException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
}

DOMElement* DetachRootElement(DOMDocument* document)
{
    DOMElement* rootElement = document->getDocumentElement();
    document->removeChild(rootElement);

    if (rootElement->getParentNode() != nullptr)
        std::cout << "Fail to detach root element out of document" << std::endl;
    else
        std::cout << "Success on detach root element" << std::endl;

    return rootElement;
}

DOMDocumentFragment* DetachRootAndAddToDocumentFragment(DOMDocument* document)
{
    DOMElement* root = DetachRootElement(document);
    DOMDocumentFragment* documentFragment = document->createDocumentFragment();
    documentFragment->appendChild(root);

    return documentFragment;
}

std::list<DOMElement*> GetElementByXpathFromDetachedElement(DOMDocument* document, DOMElement* element, const std::string& xpath)
{
    try
    {
        std::list<DOMElement*> resultList;

        // TODO: release manually

        AutoRelease<DOMXPathNSResolver> resolver(document->createNSResolver(element));
        AutoRelease<DOMXPathExpression> parsedExpression(document->createExpression(X(xpath.c_str()), resolver));

        AutoRelease<DOMXPathResult> result(
            parsedExpression->evaluate(
                element,
                DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
                nullptr
            )
        );

        size_t nLength = result->getSnapshotLength();

        if (nLength == 0)
            throw std::runtime_error("No result");

        for (size_t i = 0; i < nLength; i++)
        {
            result->snapshotItem(i);

            auto tempNode = result->getNodeValue();

            if (tempNode->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                PrintNodeType(tempNode->getNodeType());
                throw std::runtime_error("Result contain non-element node");
            }

            resultList.push_back(dynamic_cast<DOMElement*>(tempNode));
        }

        return resultList;
    }
    catch (const XQillaException& ex)
    {
        std::cout << "\n" << "XQillaException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMXPathException& ex)
    {
        std::cout << "\n" << "DOMXPathException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMException& ex)
    {
        std::cout << "\n" << "DOMException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
}

std::list<DOMElement*> GetElementByXpathFromDocumentFragment(DOMDocument* document, DOMDocumentFragment* docFragment, const std::string& xpath)
{
    try
    {
        std::list<DOMElement*> resultList;

        // TODO: release manually

        AutoRelease<DOMXPathNSResolver> resolver(document->createNSResolver(docFragment));
        AutoRelease<DOMXPathExpression> parsedExpression(document->createExpression(X(xpath.c_str()), resolver));

        AutoRelease<DOMXPathResult> result(
            parsedExpression->evaluate(
                docFragment,
                DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
                nullptr
            )
        );

        size_t nLength = result->getSnapshotLength();

        if (nLength == 0)
            throw std::runtime_error("No result");

        for (size_t i = 0; i < nLength; i++)
        {
            result->snapshotItem(i);

            auto tempNode = result->getNodeValue();

            if (tempNode->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                PrintNodeType(tempNode->getNodeType());
                throw std::runtime_error("Result contain non-element node");
            }

            resultList.push_back(dynamic_cast<DOMElement*>(tempNode));
        }

        return resultList;
    }
    catch (const XQillaException& ex)
    {
        std::cout << "\n" << "XQillaException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMXPathException& ex)
    {
        std::cout << "\n" << "DOMXPathException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
    catch (const DOMException& ex)
    {
        std::cout << "\n" << "DOMException" << std::endl;
        throw std::runtime_error(UTF8(ex.getMessage()));
    }
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
