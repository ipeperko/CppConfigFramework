/* This file is part of C++ Config Framework.
 *
 * C++ Config Framework is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * C++ Config Framework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with C++ Config
 * Framework. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \file
 *
 * Contains classes for storing a configuration node's data
 */

#ifndef CPPCONFIGFRAMEWORK_CONFIGNODEDATA_HPP
#define CPPCONFIGFRAMEWORK_CONFIGNODEDATA_HPP

// C++ Config Framework includes
#include <CppConfigFramework/ConfigNode.hpp>
#include <CppConfigFramework/DerivedObjectData.hpp>

// Qt includes
#include <QtCore/QVariant>

// System includes
#include <unordered_map>

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppConfigFramework
{

struct StringHashCalculator
{
    std::size_t operator()(const QString &value) const;
};

// -------------------------------------------------------------------------------------------------

//! This is a base class for a configuration node data
class ConfigNodeData
{
public:
    /*!
     * Alias for Array node data
     *
     * \note    The main reason for using std::list instead of std::vector is so that the references
     *          to the elements in the container are not invalidated when elements are added to or
     *          removed from the container
     */
    using ArrayNodeData = std::list<ConfigNode>;

    //! Alias for Object node data
    using ObjectNodeData = std::unordered_map<QString, ConfigNode, StringHashCalculator>;

public:
    //! Constructor
    ConfigNodeData() = default;

    //! Copy constructor
    ConfigNodeData(const ConfigNodeData &other) = delete;

    //! Move constructor
    ConfigNodeData(ConfigNodeData &&other) = delete;

    //! Destructor
    virtual ~ConfigNodeData() = default;

    //! Copy assignment operator
    ConfigNodeData &operator=(const ConfigNodeData &other) = delete;

    //! Move assignment operator
    ConfigNodeData &operator=(ConfigNodeData &&other) = delete;

    //! Creates the node data
    static std::unique_ptr<ConfigNodeData> create(const ConfigNode::Type type);

    //! Gets the node's type
    virtual ConfigNode::Type type() const = 0;

    //! Gets a reference to the Value data
    virtual const QVariant *value() const;

    //! \copydoc    ConfigNodeData::value()
    virtual QVariant *value();

    //! Gets a reference to the Array data
    virtual const ArrayNodeData *array() const;

    //! \copydoc    ConfigNodeData::array()
    virtual ArrayNodeData *array();

    //! Gets a reference to the Object data
    virtual const ObjectNodeData *object() const;

    //! \copydoc    ConfigNodeData::object()
    virtual ObjectNodeData *object();

    //! Gets a reference to the NodeReference data
    virtual const QString *nodeReference() const;

    //! \copydoc    ConfigNodeData::nodeReference()
    virtual QString *nodeReference();

    //! Gets a reference to the DerivedObject data
    virtual const DerivedObjectData *derivedObject() const;

    //! \copydoc    ConfigNodeData::derivedObject()
    virtual DerivedObjectData *derivedObject();
};

// -------------------------------------------------------------------------------------------------

//! This is a class for a Null configuration node data
class ConfigNodeNullData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeNullData() = default;

    //! Copy constructor
    ConfigNodeNullData(const ConfigNodeNullData &other) = delete;

    //! Move constructor
    ConfigNodeNullData(ConfigNodeNullData &&other) = delete;

    //! Destructor
    ~ConfigNodeNullData() override = default;

    //! Copy assignment operator
    ConfigNodeNullData &operator=(const ConfigNodeNullData &other) = delete;

    //! Move assignment operator
    ConfigNodeNullData &operator=(ConfigNodeNullData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;
};

// -------------------------------------------------------------------------------------------------

//! This is a class for a Value configuration node data
class ConfigNodeValueData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeValueData() = default;

    //! Copy constructor
    ConfigNodeValueData(const ConfigNodeValueData &other) = delete;

    //! Move constructor
    ConfigNodeValueData(ConfigNodeValueData &&other) = delete;

    //! Destructor
    ~ConfigNodeValueData() override = default;

    //! Copy assignment operator
    ConfigNodeValueData &operator=(const ConfigNodeValueData &other) = delete;

    //! Move assignment operator
    ConfigNodeValueData &operator=(ConfigNodeValueData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;

    //! \copydoc    ConfigNodeData::value()
    const QVariant *value() const override;

    //! \copydoc    ConfigNodeData::value()
    QVariant *value() override;

private:
    //! Actual data
    QVariant m_value;
};

// -------------------------------------------------------------------------------------------------

//! This is a class for an Array configuration node data
class ConfigNodeArrayData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeArrayData() = default;

    //! Copy constructor
    ConfigNodeArrayData(const ConfigNodeArrayData &other) = delete;

    //! Move constructor
    ConfigNodeArrayData(ConfigNodeArrayData &&other) = delete;

    //! Destructor
    ~ConfigNodeArrayData() override = default;

    //! Copy assignment operator
    ConfigNodeArrayData &operator=(const ConfigNodeArrayData &other) = delete;

    //! Move assignment operator
    ConfigNodeArrayData &operator=(ConfigNodeArrayData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;

    //! \copydoc    ConfigNodeData::array()
    const ArrayNodeData *array() const override;

    //! \copydoc    ConfigNodeData::array()
    ArrayNodeData *array() override;

private:
    //! Actual data
    ArrayNodeData m_array;
};

// -------------------------------------------------------------------------------------------------

//! This is a class for an Object configuration node data
class ConfigNodeObjectData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeObjectData() = default;

    //! Copy constructor
    ConfigNodeObjectData(const ConfigNodeObjectData &other) = delete;

    //! Move constructor
    ConfigNodeObjectData(ConfigNodeObjectData &&other) = delete;

    //! Destructor
    ~ConfigNodeObjectData() override = default;

    //! Copy assignment operator
    ConfigNodeObjectData &operator=(const ConfigNodeObjectData &other) = delete;

    //! Move assignment operator
    ConfigNodeObjectData &operator=(ConfigNodeObjectData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;

    //! \copydoc    ConfigNodeData::object()
    const ObjectNodeData *object() const override;

    //! \copydoc    ConfigNodeData::object()
    ObjectNodeData *object() override;

private:
    //! Actual data
    ObjectNodeData m_object;
};

// -------------------------------------------------------------------------------------------------

//! This is a class for an NodeReference configuration node data
class ConfigNodeReferenceData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeReferenceData() = default;

    //! Copy constructor
    ConfigNodeReferenceData(const ConfigNodeReferenceData &other) = delete;

    //! Move constructor
    ConfigNodeReferenceData(ConfigNodeReferenceData &&other) = delete;

    //! Destructor
    ~ConfigNodeReferenceData() override = default;

    //! Copy assignment operator
    ConfigNodeReferenceData &operator=(const ConfigNodeReferenceData &other) = delete;

    //! Move assignment operator
    ConfigNodeReferenceData &operator=(ConfigNodeReferenceData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;

    //! \copydoc    ConfigNodeData::nodeReference()
    const QString *nodeReference() const override;

    //! \copydoc    ConfigNodeData::nodeReference()
    QString *nodeReference() override;

private:
    //! Actual data
    QString m_nodeReference;
};

// -------------------------------------------------------------------------------------------------

//! This is a class for an DerivedObject configuration node data
class ConfigNodeDerivedObjectData : public ConfigNodeData
{
public:
    //! Constructor
    ConfigNodeDerivedObjectData() = default;

    //! Copy constructor
    ConfigNodeDerivedObjectData(const ConfigNodeDerivedObjectData &other) = delete;

    //! Move constructor
    ConfigNodeDerivedObjectData(ConfigNodeDerivedObjectData &&other) = delete;

    //! Destructor
    ~ConfigNodeDerivedObjectData() override = default;

    //! Copy assignment operator
    ConfigNodeDerivedObjectData &operator=(const ConfigNodeDerivedObjectData &other) = delete;

    //! Move assignment operator
    ConfigNodeDerivedObjectData &operator=(ConfigNodeDerivedObjectData &&other) = delete;

    //! \copydoc    ConfigNodeData::type()
    ConfigNode::Type type() const override;

    //! \copydoc    ConfigNodeData::derivedObject()
    const DerivedObjectData *derivedObject() const override;

    //! \copydoc    ConfigNodeData::derivedObject()
    DerivedObjectData *derivedObject() override;

private:
    //! Actual data
    DerivedObjectData m_data;
};

} // namespace CppConfigFramework

#endif // CPPCONFIGFRAMEWORK_CONFIGNODEDATA_HPP
