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
 * Contains a class for reading configuration files
 */

// Own header
#include <CppConfigFramework/ConfigReader.hpp>

// C++ Config Framework includes
#include <CppConfigFramework/ConfigNode.hpp>
#include "DebugHelper.hpp"

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QtDebug>

// System includes

// Forward declarations

// Macros
#define DEBUG_METHOD(METHOD)      CPPCONFIGFRAMEWORK_DEBUG_METHOD("ConfigReader::" METHOD)
#define DEBUG_METHOD_IMPL(METHOD) CPPCONFIGFRAMEWORK_DEBUG_METHOD("ConfigReader::Impl::" METHOD)

// -------------------------------------------------------------------------------------------------

namespace CppConfigFramework
{

// -------------------------------------------------------------------------------------------------
// Implementation
// -------------------------------------------------------------------------------------------------

/*!
 * Private implementation for ConfigReader class
 */
class ConfigReader::Impl
{
public:
    //! \copydoc    ConfigReader::read()
    std::unique_ptr<ConfigNode> read(const QString &filePath,
                                     const QString &sourceNode,
                                     const QString &destinationNode);

private:
    /*!
     * Reads the 'includes' member of the configuration file
     *
     * \param   rootObject  Root JSON Object
     *
     * \retval  true    Success
     * \retval  false   Failure
     */
    std::unique_ptr<ConfigNode> readIncludesMember(const QJsonObject &rootObject);

    /*!
     * Reads the 'config' member of the configuration file
     *
     * \param   rootObject  Root JSON Object
     *
     * \retval  true    Success
     * \retval  false   Failure
     */
    std::unique_ptr<ConfigNode> readConfigMember(const QJsonObject &rootObject);

    /*!
     * Reads the JSON Value
     *
     * \param   jsonValue       JSON Value
     * \param   currentNodePath Current node path
     *
     * \return  Configuration node instance or in case of failure a null pointer
     */
    static std::unique_ptr<ConfigNode> readJsonValue(const QJsonValue &jsonValue,
                                                     const QString &currentNodePath);

    /*!
     * Reads the JSON Array
     *
     * \param   jsonArray       JSON Array
     * \param   currentNodePath Current node path
     *
     * \return  Configuration node instance or in case of failure a null pointer
     */
    static std::unique_ptr<ConfigNode> readJsonArray(const QJsonArray &jsonArray,
                                                     const QString &currentNodePath);

    /*!
     * Reads the JSON Object
     *
     * \param   jsonObject      JSON Object
     * \param   currentNodePath Current node path
     *
     * \return  Configuration node instance or in case of failure a null pointer
     */
    static std::unique_ptr<ConfigNode> readJsonObject(const QJsonObject &jsonObject,
                                                      const QString &currentNodePath);

    //! Node path to the node that needs to be extracted from this configuration file
    QString m_sourceNode;

    //! Node path to the destination node where the result needs to be stored
    QString m_destinationNode;
};

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::read(const QString &filePath,
                                                     const QString &sourceNode,
                                                     const QString &destinationNode)
{
    // Validate source node
    if ((!ConfigNode::isAbsoluteNodePath(sourceNode)) ||
        (!ConfigNode::validateNodePath(sourceNode)))
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: invalid source node path:" << sourceNode;
        return {};
    }

    m_sourceNode = sourceNode;

    // Validate destination node
    if ((!ConfigNode::isAbsoluteNodePath(destinationNode)) ||
        (!ConfigNode::validateNodePath(destinationNode)))
    {
        qDebug() << DEBUG_METHOD_IMPL("read")
                 << "Error: invalid destination node path:" << destinationNode;
        return {};
    }

    m_destinationNode = destinationNode;

    // Open file
    if (!QFile::exists(filePath))
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: file at path was not found:" << filePath;
        return {};
    }

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: failed to open file at path:" << filePath;
        return {};
    }

    // Read the contents (JSON format)
    QJsonParseError error;
    const QByteArray fileContents = file.readAll();
    const auto doc = QJsonDocument::fromJson(fileContents, &error);

    if (error.error != QJsonParseError::NoError)
    {
        constexpr int contextMaxLength = 20;
        const int contextBeforeIndex = std::max(0, error.offset - contextMaxLength);
        const int contextBeforeLength = std::min(error.offset, contextMaxLength);

        qDebug() << DEBUG_METHOD_IMPL("read")
                 << "Error: failed to parse the file contents:"
                    "\n    file path:" << filePath
                 << "\n    offset:" << error.offset
                 << "\n    error:" << error.errorString()
                 << "\n    context before error:" << fileContents.mid(contextBeforeIndex,
                                                                      contextBeforeLength)
                 << "\n    context at error:" << fileContents.mid(error.offset, contextMaxLength);
        return {};
    }

    if (!doc.isObject())
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: failed to read includes";
        return {};
    }

    const auto rootObject = doc.object();

    // TODO: add "environment variables" member?

    // Read 'includes' member
    auto includesConfig = readIncludesMember(rootObject);

    if (!includesConfig)
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: failed to read includes";
        return {};
    }

    // Read 'config' member
    auto config = readConfigMember(rootObject);

    if (!config)
    {
        qDebug() << DEBUG_METHOD_IMPL("read") << "Error: failed to read includes";
        return {};
    }

    if (!config->apply(*includesConfig))
    {
        return {};
    }

    // TODO: resolve all references and inheritance?

    return config;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::readIncludesMember(const QJsonObject &rootObject)
{
    // Check if the root object has any includes
    const auto includesValue = rootObject.value(QStringLiteral("includes"));

    if (includesValue.isNull())
    {
        // No includes
        return std::make_unique<ConfigNode>(ConfigNode::Type::Null);
    }

    if (!includesValue.isArray())
    {
        qDebug() << DEBUG_METHOD_IMPL("readIncludesMember")
                 << "Error: the 'includes' member in the root JSON Object is not a JSON array!";
        return {};
    }

    // Iterate over all includes and extract their value
    auto includesConfig = std::make_unique<ConfigNode>(ConfigNode::Type::Object);
    const auto includesArray = includesValue.toArray();

    for (int i = 0; i < includesArray.size(); i++)
    {
        // Include item must be a JSON Object
        const auto includeValue = includesArray.at(i);

        if (!includeValue.isObject())
        {
            qDebug() << DEBUG_METHOD_IMPL("readIncludesMember")
                     << QString("Error: element at index [%1] is not a JSON Object!").arg(i);
            return {};
        }

        // TODO: implement
        //const auto include = includeValue.toObject();

        // TODO: check configuration file type

        // Extract file path
        // TODO: implement

        // Extract source node
        // TODO: implement

        // Extract destination node
        // TODO: implement

        // Read config file
        // TODO: implement

        // Merge the config file contents to our own configuration parameter data structure
        // TODO: implement --> includesConfig->apply(config)
    }

    return includesConfig;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::readConfigMember(const QJsonObject &rootObject)
{
    // The root object must contain the 'config' member (but it can be an empty object)
    const auto configValue = rootObject.value(QStringLiteral("config"));

    if (configValue.isNull())
    {
        // No configuration
        return std::make_unique<ConfigNode>(ConfigNode::Type::Null);
    }

    if (!configValue.isObject())
    {
        qDebug() << DEBUG_METHOD_IMPL("readConfigMember")
                 << "Error: the 'config' member in the root JSON Object is not a JSON Object!";
        return {};
    }

    // Read 'config' object
    auto config = readJsonObject(configValue.toObject(), QStringLiteral("/"));

    if (!config)
    {
        qDebug() << DEBUG_METHOD_IMPL("readConfigMember")
                 << "Error: failed to read the 'config' member in the root JSON Object!";
        return {};
    }

    return config;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::readJsonValue(const QJsonValue &jsonValue,
                                                              const QString &currentNodePath)
{
    switch (jsonValue.type())
    {
        case QJsonValue::Null:
        {
            return std::make_unique<ConfigNode>();
        }

        case QJsonValue::Bool:
        case QJsonValue::Double:
        case QJsonValue::String:
        {
            auto valueNode = std::make_unique<ConfigNode>(ConfigNode::Type::Value);
            valueNode->setValue(jsonValue.toVariant());
            return valueNode;
        }

        case QJsonValue::Array:
        {
            return readJsonArray(jsonValue.toArray(), currentNodePath);
        }

        case QJsonValue::Object:
        {
            return readJsonObject(jsonValue.toObject(), currentNodePath);
        }

        case QJsonValue::Undefined:
        {
            qDebug() << DEBUG_METHOD_IMPL("readJsonValue")
                     << "Error: unexpected 'Undefined' JSON value at path:" << currentNodePath;
            return {};
        }
    }

    qDebug() << DEBUG_METHOD_IMPL("readJsonValue")
             << "Error: unsupported JSON value type at path:" << currentNodePath;
    return {};
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::readJsonArray(const QJsonArray &jsonArray,
                                                              const QString &currentNodePath)
{
    auto arrayNode = std::make_unique<ConfigNode>(ConfigNode::Type::Array);

    for (int i = 0; i < jsonArray.size(); i++)
    {
        auto elementNode = readJsonValue(jsonArray.at(i),
                                         ConfigNode::appendNodeToPath(currentNodePath,
                                                                      QString::number(i)));

        if (!elementNode)
        {
            return {};
        }

        arrayNode->appendElement(std::move(*elementNode));
    }

    return arrayNode;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::Impl::readJsonObject(const QJsonObject &jsonObject,
                                                               const QString &currentNodePath)
{
    auto objectNode = std::make_unique<ConfigNode>(ConfigNode::Type::Object);

    for (auto it = jsonObject.begin(); it != jsonObject.end(); it++)
    {
        const QString memberName = it.key();

        if (!ConfigNode::validateNodeName(memberName))
        {
            // TODO: make an exception for reference types!
            qDebug() << DEBUG_METHOD_IMPL("readJsonObject")
                     << "Error: invalid member name:" << memberName;
            return {};
        }

        auto memberNode = readJsonValue(it.value(),
                                        ConfigNode::appendNodeToPath(currentNodePath, memberName));

        if (!memberNode)
        {
            return {};
        }

        objectNode->setMember(memberName, std::move(*memberNode));
    }

    return objectNode;
}

// -------------------------------------------------------------------------------------------------
// API
// -------------------------------------------------------------------------------------------------

ConfigReader::ConfigReader()
    : m_pimpl(std::make_unique<Impl>())
{
}

// -------------------------------------------------------------------------------------------------

ConfigReader::ConfigReader(ConfigReader &&other) noexcept = default;

// -------------------------------------------------------------------------------------------------

ConfigReader::~ConfigReader() = default;

// -------------------------------------------------------------------------------------------------

ConfigReader &ConfigReader::operator=(ConfigReader &&other) noexcept = default;

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigNode> ConfigReader::read(const QString &filePath,
                                               const QString &sourceNode,
                                               const QString &destinationNode)
{
    return impl()->read(filePath, sourceNode, destinationNode);
}

} // namespace CppConfigFramework