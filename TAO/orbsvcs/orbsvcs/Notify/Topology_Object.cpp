// $Id$

#include "Topology_Object.h"

#if ! defined (__ACE_INLINE__)
#include "Topology_Object.inl"
#endif /* __ACE_INLINE__ */

// question: is there a race_conditon with self_changed and children_changed?
// answer: toplogy_changed and/or children_changed must be set after the change is
// made, and before the call to child_change.
// self_changed and children_changed must be cleared before this object and its
// children have been saved in Topology_Object::save_persistent ().
// If these rules are followed, the only risk is a (harmless) extra save.

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

namespace TAO_Notify
{
  const char TOPOLOGY_ID_NAME[] = "TopologyID";

  Topology_Savable::~Topology_Savable (void)
  {
  }

  void
  Topology_Savable::reconnect (ACE_ENV_SINGLE_ARG_DECL_NOT_USED)
  {
  }

  Topology_Object::Topology_Object ()
    : TAO_Notify_Object ()
    , Topology_Savable ()
    , self_changed_ (false)
    , children_changed_ (false)
    , topology_parent_ (0)
  {
  }

  Topology_Object::~Topology_Object ()
  {
  }

  void
  Topology_Object::initialize (Topology_Parent* topology_parent ACE_ENV_ARG_DECL_NOT_USED)
  {
    ACE_ASSERT (topology_parent != 0 && this->topology_parent_ == 0);
    this->topology_parent_ = topology_parent;
    TAO_Notify_Object::initialize (topology_parent);
  }

  Topology_Parent *
  Topology_Object::topology_parent () const
  {
    return this->topology_parent_;
  }


  Topology_Object *
  Topology_Object::load_child (const ACE_CString & /*type*/,
    CORBA::Long /* id */,
    const NVPList& /* attrs */
    ACE_ENV_ARG_DECL_NOT_USED)
  {
    return 0;
  }

  bool
  Topology_Object::is_persistent () const
  {
    bool result = false;
    if (this->qos_properties_.event_reliability().is_valid ())
    {
      result = CosNotification::Persistent == this->qos_properties_.event_reliability().value ();
    }
    else if (this->topology_parent () != 0)
    {
      result = this->topology_parent ()->is_persistent ();
    }
    return result;
  }

  bool
  Topology_Object::self_change (ACE_ENV_SINGLE_ARG_DECL)
  {
    this->self_changed_ = true;
    return send_change (ACE_ENV_SINGLE_ARG_PARAMETER);
  }

  bool
  Topology_Object::send_change (ACE_ENV_SINGLE_ARG_DECL)
  {
    bool saving = false;
    if (is_persistent ())
    {
      while (this->self_changed_ || this->children_changed_)
      {
        saving = this->change_to_parent (ACE_ENV_SINGLE_ARG_PARAMETER);
        ACE_CHECK_RETURN (false);
        if (!saving)
        {
          this->self_changed_ = false;
          this->children_changed_ = false;
        }
      }
    }
    else
    {
      this->self_changed_ = false;
      this->children_changed_ = false;
    }
    return saving;
  }

  bool
  Topology_Object::change_to_parent (ACE_ENV_SINGLE_ARG_DECL)
  {
    bool result = false;
    Topology_Parent * parent = this->topology_parent();
    if (parent != 0)
    {
      result = parent->child_change(ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_CHECK_RETURN (false);
    }
    return result;
  }

  void
  Topology_Object::get_id_path (TAO_Notify::IdVec & id_path) const
  {
    if (this->topology_parent() != 0)
    {
      this->topology_parent()->get_id_path (id_path);
    }
    id_path.push_back (this->get_id ());
  }

  TAO_Notify_Object::ID
  Topology_Object::get_id () const
  {
    // If this assert triggers then implement the
    // get_id method in the actual class
    // derived from Topology_Object
    // or else figure out why this method was called
    // on an object that doesn't have an id.
    ACE_ASSERT (false);
    // if it is called in a release build, provide 'em a value
    return -1;
  }

} // namespace TAO_Notify

TAO_END_VERSIONED_NAMESPACE_DECL
