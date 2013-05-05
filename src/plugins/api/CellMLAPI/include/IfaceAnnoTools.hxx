/* This file is automatically generated from AnnoTools.idl
 * DO NOT EDIT DIRECTLY OR CHANGES WILL BE LOST!
 */
#ifndef _GUARD_IFACE_AnnoTools
#define _GUARD_IFACE_AnnoTools
#include "cda_compiler_support.h"
#ifdef MODULE_CONTAINS_AnnoTools
#define PUBLIC_AnnoTools_PRE CDA_EXPORT_PRE
#define PUBLIC_AnnoTools_POST CDA_EXPORT_POST
#else
#define PUBLIC_AnnoTools_PRE CDA_IMPORT_PRE
#define PUBLIC_AnnoTools_POST CDA_IMPORT_POST
#endif
#include "Ifacexpcom.hxx"
#include "IfaceDOM_APISPEC.hxx"
#include "IfaceMathML_content_APISPEC.hxx"
#include "IfaceCellML_APISPEC.hxx"
namespace iface
{
  namespace cellml_services
  {
    PUBLIC_AnnoTools_PRE 
    class  PUBLIC_AnnoTools_POST StringAnnotation
     : public virtual iface::cellml_api::UserData
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::StringAnnotation"; }
      virtual ~StringAnnotation() {}
      virtual std::wstring value() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
      virtual void value(const std::wstring& attr) throw(std::exception&) = 0;
    };
    PUBLIC_AnnoTools_PRE 
    class  PUBLIC_AnnoTools_POST ObjectAnnotation
     : public virtual iface::cellml_api::UserData
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::ObjectAnnotation"; }
      virtual ~ObjectAnnotation() {}
      virtual already_AddRefd<iface::XPCOM::IObject>  value() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
      virtual void value(iface::XPCOM::IObject* attr) throw(std::exception&) = 0;
    };
    PUBLIC_AnnoTools_PRE 
    class  PUBLIC_AnnoTools_POST AnnotationSet
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::AnnotationSet"; }
      virtual ~AnnotationSet() {}
      virtual std::wstring prefixURI() throw(std::exception&)  WARN_IF_RETURN_UNUSED = 0;
      virtual void setStringAnnotation(iface::cellml_api::CellMLElement* element, const std::wstring& key, const std::wstring& value) throw(std::exception&) = 0;
      virtual std::wstring getStringAnnotation(iface::cellml_api::CellMLElement* element, const std::wstring& key) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual std::wstring getStringAnnotationWithDefault(iface::cellml_api::CellMLElement* element, const std::wstring& key, const std::wstring& defval) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual void setObjectAnnotation(iface::cellml_api::CellMLElement* element, const std::wstring& key, iface::XPCOM::IObject* value) throw(std::exception&) = 0;
      virtual already_AddRefd<iface::XPCOM::IObject>  getObjectAnnotation(iface::cellml_api::CellMLElement* element, const std::wstring& key) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
      virtual already_AddRefd<iface::XPCOM::IObject>  getObjectAnnotationWithDefault(iface::cellml_api::CellMLElement* element, const std::wstring& key, iface::XPCOM::IObject* defval) throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
    };
    PUBLIC_AnnoTools_PRE 
    class  PUBLIC_AnnoTools_POST AnnotationToolService
     : public virtual iface::XPCOM::IObject
    {
    public:
      static const char* INTERFACE_NAME() { return "cellml_services::AnnotationToolService"; }
      virtual ~AnnotationToolService() {}
      virtual already_AddRefd<iface::cellml_services::AnnotationSet>  createAnnotationSet() throw(std::exception&) WARN_IF_RETURN_UNUSED = 0;
    };
  };
};
#undef PUBLIC_AnnoTools_PRE
#undef PUBLIC_AnnoTools_POST
#endif // guard
