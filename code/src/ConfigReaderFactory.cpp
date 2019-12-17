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
 * Contains the factory for classes that can read different type of configuration files
 */

// Own header
#include <CppConfigFramework/ConfigReaderFactory.hpp>

// C++ Config Framework includes
#include <CppConfigFramework/ConfigObjectNode.hpp>
#include <CppConfigFramework/ConfigReader.hpp>

// Qt includes
#include <QtCore/QStringBuilder>

// System includes

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppConfigFramework
{

ConfigReaderFactory *ConfigReaderFactory::instance()
{
    static ConfigReaderFactory factory;

    return &factory;
}

// -------------------------------------------------------------------------------------------------

bool ConfigReaderFactory::registerConfigReader(const QString &type,
                                               std::unique_ptr<ConfigReaderBase> &&configReader)
{
    if (type.isEmpty() || (!configReader))
    {
        return false;
    }

    m_configReaders[type] = std::move(configReader);
    return true;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigObjectNode> ConfigReaderFactory::readConfig(
        const QString &type,
        const QDir &workingDir,
        const ConfigNodePath &destinationNodePath,
        const QVariantMap &otherParameters,
        const std::vector<const ConfigObjectNode *> &externalConfigs,
        EnvironmentVariables *environmentVariables,
        QString *error) const
{
    // Get the specified type of config reader
    auto it = m_configReaders.find(type);

    if (it == m_configReaders.end())
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("Unsupported configuration type: ") % type;
        }
        return {};
    }

    auto *configReader = it->second.get();

    // Read the config
    return configReader->read(workingDir,
                              destinationNodePath,
                              otherParameters,
                              externalConfigs,
                              environmentVariables,
                              error);
}

// -------------------------------------------------------------------------------------------------

ConfigReaderFactory::ConfigReaderFactory()
{
    registerConfigReader(QStringLiteral("CppConfigFramework"), std::make_unique<ConfigReader>());
}

} // namespace CppConfigFramework