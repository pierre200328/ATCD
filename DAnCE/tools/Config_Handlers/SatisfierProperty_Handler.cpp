#include "SatisfierProperty_Handler.h"
#include "Any_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
#include "Utils/Exceptions.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    SatisfierProperty_Handler::SatisfierProperty_Handler (void)
    {
      DANCE_TRACE("SatisfierProperty_Handler::constructor");
    }

    SatisfierProperty_Handler::~SatisfierProperty_Handler (void)
    {
      DANCE_TRACE("SatisfierProperty_Handler::destructor");
    }

    void
    SatisfierProperty_Handler::handle_sat_property (
                                                 const SatisfierProperty& desc,
                                                 Deployment::SatisfierProperty& toconfig)
    {
      DANCE_TRACE("SatisfierProperty_Handler::get_sat_property");

      toconfig.name = ACE_TEXT_ALWAYS_CHAR (desc.name ().c_str ());

      switch (desc.kind ().integral ())
        {
        case SatisfierPropertyKind::Quantity_l:
          toconfig.kind = Deployment::Quantity;
          break;

        case SatisfierPropertyKind::Capacity_l:
          toconfig.kind = Deployment::Capacity;
          break;

        case SatisfierPropertyKind::Minimum_l:
          toconfig.kind = Deployment::Minimum;
          break;

        case SatisfierPropertyKind::Maximum_l:
          toconfig.kind = Deployment::Maximum;
          break;

        case SatisfierPropertyKind::Attribute_l:
          toconfig.kind = Deployment::Attribute;
          break;

        case SatisfierPropertyKind::Selection_l:
          toconfig.kind = Deployment::Selection;
          break;

        default:
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR, ACE_TEXT ("Unknown SatisfierPropertyKind\n")));
          throw Config_Error (desc.name (),
            ACE_TEXT ("Unknown SatisfierPropertyKind"));

        }

      toconfig.dynamic = desc.dynamic ();

      try
        {
          Any_Handler::extract_into_any (desc.value (),
                                         toconfig.value);
        }
      catch (Config_Error &ex)
        {
          ex.name_ = desc.name ();
          throw ex;
        }
    }

    SatisfierProperty
    SatisfierProperty_Handler::get_sat_property (
                                                 const Deployment::SatisfierProperty& src)
    {
      DANCE_TRACE("SatisfierProperty_Handler::get_sat_property - reverse");

      ::XMLSchema::string< ACE_TCHAR > name (ACE_TEXT_CHAR_TO_TCHAR ((src.name)));

      const SatisfierPropertyKind *spk = 0;

      switch (src.kind)
        {
        case Deployment::Quantity:
          spk = &SatisfierPropertyKind::Quantity;
          break;

        case Deployment::Capacity:
          spk = &SatisfierPropertyKind::Capacity;
          break;

        case Deployment::Minimum:
          spk = &SatisfierPropertyKind::Minimum;
          break;

        case Deployment::Maximum:
          spk = &SatisfierPropertyKind::Maximum;
          break;

        default:
          throw Plan_Error (ACE_TEXT ("Invalid SatisfierPropertyKind"));
        }

      bool dynamic = src.dynamic;

      Any value (Any_Handler::get_any (src.value));

      return SatisfierProperty (name, *spk, dynamic, value);
    }

  }
}
