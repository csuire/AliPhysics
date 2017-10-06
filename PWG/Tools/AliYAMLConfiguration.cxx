//
//

#include "AliYAMLConfiguration.h"

#include <cstdio>
#include <fstream>

#include <TSystem.h>
#include <TGrid.h>
#include <TFile.h>
#include <TUUID.h>

/// \cond CLASSIMP
ClassImp(AliYAMLConfiguration);
/// \endcond

/**
 * Default constructor.
 *
 * @param[in] prefixString Prefix to remove when gettnig a property.
 * @param[in] delimiterCharacter Character that delimits between each part of the YAML path.
 */
AliYAMLConfiguration::AliYAMLConfiguration(const std::string prefixString, const std::string delimiterCharacter):
  TObject(),
  fConfigurations(),
  fConfigurationsStrings(),
  fInitialized(false),
  fPrefixString(prefixString),
  fDelimiter(delimiterCharacter)
{
}

/**
 * Create an empty YAML node and adds it to the available configurations.
 *
 * @param[in] configurationName Name of the YAML node. The node will be stored under this name.
 *
 * @return True if the configuration was added successfully.
 */
bool AliYAMLConfiguration::AddEmptyConfiguration(const std::string & configurationName)
{
  YAML::Node node;
  AliInfoStream() << "Adding configuration \"" << configurationName << "\" as an empty YAML node.\n";
  return AddConfiguration(node, configurationName);
}

/**
 * Add YAML configuration at a given filename to the available configurations.
 *
 * @param[in] configurationFilename Filename of the YAML configuration file to be added.
 * @param[in] configurationName Name of the YAML node. The node will be stored under this name.
 *
 * @return True if the configuration was added successfully.
 */
bool AliYAMLConfiguration::AddConfiguration(std::string configurationFilename, std::string configurationName)
{
  SetupReadingConfigurationFilePath(configurationFilename, configurationName);

  // Add file
  if (DoesFileExist(configurationFilename) == false) {
    AliErrorStream() << "Configuration filename \"" << configurationFilename << "\" does not exist!\n";
    return false;
  }

  // Create node from filename
  auto node = YAML::LoadFile(configurationFilename);

  if (node.IsNull() == true) {
    AliErrorStream() << "The node at configuration filename \"" << configurationFilename << "\" is null and will not be added!\n";
    return false;
  }

  AliInfoStream() << "Adding configuration \"" << configurationName << "\" located at \"" << configurationFilename << "\".\n";
  return AddConfiguration(node, configurationName);
}

/**
 * Add a YAML node to the available configurations.
 *
 * @param[in] node YAML node to be added.
 * @param[in] configurationName Name of the YAML node. The node will be stored under this name.
 *
 * @return True if the configuration was added successfully.
 */
bool AliYAMLConfiguration::AddConfiguration(const YAML::Node node, std::string configurationName)
{
  if (configurationName == "")
  {
    // Attempt to get name from node
    // It is fine if the node is empty
    GetPropertyFromNode(node, "name", configurationName);
  }

  if (configurationName == "") {
    AliErrorStream() << "Could not determine a name for the configuration:\n\"\n" << node << "\n\" Configuration will not be added!\n";
    return false;
  }

  // Add the configuration
  AliDebugStream(2) << "Adding configuration \"" << configurationName << "\".\n";
  fConfigurations.push_back(std::make_pair(configurationName, node));
  return true;
}

/**
 * Write a YAML configuration node to file.
 *
 * @param[in] index Index of the YAML configuration node.
 * @param[in] filename Filename to write the node to.
 *
 * @return True if write was successful.
 */
bool AliYAMLConfiguration::WriteConfiguration(const std::string & filename, const unsigned int index) const
{
  // Write to a local temp filename
  TUUID tempUUID;
  std::string localFilename = tempUUID.AsString();
  localFilename += ".yaml";

  auto configPair = fConfigurations.at(index);
  std::ofstream outputFile(localFilename);
  outputFile << configPair.second;
  outputFile.close();

  // Hanldes writing to places like AliEn
  WriteConfigurationToFilePath(localFilename, filename);

  remove(localFilename.c_str());

  return true;
}

/**
 * Write a YAML configuration node to file.
 *
 * @param[in] configurationName Name of the YAML configuration node.
 * @param[in] filename Filename to write the node to.
 *
 * @return True if write was successful.
 */
bool AliYAMLConfiguration::WriteConfiguration(const std::string & filename, const std::string & configurationName) const
{
  return WriteConfiguration(filename, GetConfigurationIndexByName(configurationName, fConfigurations));
}

/**
 * Checks if a file exists. This is done inline to make it efficient.
 * See: https://stackoverflow.com/a/19841704
 *
 * @param filename String containing the filename of the file to check.
 *
 * @return True if the file exists.
 */
inline bool AliYAMLConfiguration::DoesFileExist(const std::string & filename) const
{
  std::ifstream inFile(filename);
  return inFile.good();
}

/**
 * Handles setting up the configuration file to be opened, including in AliPhysics and on the grid.
 * Cannot just use TFile::Open() because the YAML file is just text as opposed to a root file.
 * In the case of a file on the grid, it is copied locally.
 *
 * @param[in,out] filename Name of the file to be open
 * @param[in] fileIdentifier Additional file identifier to add onto the file name
 */
void AliYAMLConfiguration::SetupReadingConfigurationFilePath(std::string & filename, const std::string & fileIdentifier) const
{
  if (filename != "")
  {
    // Handle if in AliPhysics and includes $ALICE_PHYSICS
    filename = gSystem->ExpandPathName(filename.c_str());

    // Handle grid
    if(filename.find("alien://") != std::string::npos)
    {
      AliDebug(2, TString::Format("Opening file \"%s\" on the grid!", filename.c_str()));
      // Initialize AliEn connection if needed
      if (!gGrid) {
        TGrid::Connect("alien://");
      }

      // Determine the local filename and copy file to local directory
      std::string localFilename = gSystem->BaseName(filename.c_str());
      // Add identifier if it's not an empty string
      if (fileIdentifier != "") {
        localFilename = fileIdentifier + "." + localFilename;
      }
      // Add UUID to ensure there are no conflicts if multiple yaml configs have the same configuration file name
      TUUID tempUUID;
      localFilename = "." + localFilename;
      localFilename = tempUUID.AsString() + localFilename;

      // Copy file
      TFile::Cp(filename.c_str(), localFilename.c_str());

      // yaml-cpp should only open the local file
      filename = localFilename;
    }
  }
}

/**
 * Write a selected YAML configuration to file. Practically, it copies a local file to the desired location]
 * to ensure seamless access to AliEn.
 *
 * @param[in] filename Filename to which the configuration should be written.
 * @param[in] localFilename Filename where the configuration was written locally.
 */
void AliYAMLConfiguration::WriteConfigurationToFilePath(const std::string & localFilename, std::string filename) const
{
  bool cannotWriteFile = false;
  if (localFilename == "") {
    AliErrorStream() << "Local filename is null, so the file cannot be written!\n";
    cannotWriteFile = true;
  }
  if (filename == "") {
    AliErrorStream() << "Filename is null, so the file cannot be written\n";
    cannotWriteFile = true;
  }

  if (cannotWriteFile == false) {
    // Handle if in AliPhysics and includes $ALICE_PHYSICS
    filename = gSystem->ExpandPathName(filename.c_str());

    // Handle grid
    if(filename.find("alien://") != std::string::npos)
    {
      AliDebugStream(2) << "Writing file \"" << filename << "\" on the grid!\n";
      // Initialize AliEn connection if needed
      if (!gGrid) {
        TGrid::Connect("alien://");
      }
    }

    // Copy file
    AliDebugStream(2) << "Copying localFilename \"" << localFilename << "\" to filename \"" << filename << "\".\n";
    TFile::Cp(localFilename.c_str(), filename.c_str());
  }
}

/**
 * Initialize configurations.
 *
 * This includes storing the contents of the YAML configurations
 * into strings so that they can be streamed to the grid.
 * (yaml-cpp objects do not work properly with ROOT streamers).
 *
 * NOTE: fConfigurationInitialized is set to true if the function is successful.
 */
bool AliYAMLConfiguration::Initialize()
{
  // Copy all configurations to their respective strings.
  // That way, the strings will be successfully streamed by ROOT and the YAML nodes can be re-initialized on the grid.
  std::stringstream tempSS;
  for (auto & configPair : fConfigurations)
  {
    tempSS.str("");
    tempSS << configPair.second;
    fConfigurationsStrings.push_back(std::make_pair(configPair.first, tempSS.str()));
  }

  fInitialized = true;

  return fInitialized;
}

/**
 * Reinitialize the configurations from the strings after streaming. This is required because the YAML
 * node objects cannot be streamed.
 *
 * @return True if the configurations were re-initialized.
 */
bool AliYAMLConfiguration::Reinitialize()
{
  bool returnValue = false;

  // If they were not streamed then the size of the vectors should not match
  if (fConfigurations.size() != fConfigurationsStrings.size())
  {
    if (fInitialized == false)
    {
      // Not initialized, so the strings do not represent the nodes
      AliFatalGeneral("AliYAMLConfiguration", "Attempted to re-initialize the YAML nodes, but the string based configurations are not available. Did you remember to call initialize?");
    }

    for (const auto & configStrPair : fConfigurationsStrings)
    {
      YAML::Node node = YAML::Load(configStrPair.second);
      fConfigurations.push_back(std::make_pair(configStrPair.first, node));
    }

    returnValue = true;
  }

  return returnValue;
}

/**
 * Check if value is a shared parameter, meaning we should look
 * at another node. Also edits the input string to remove "sharedParameters:"
 * if it exists, making it ready for use.
 *
 * @param[in] value String containing the string value return by the parameter.
 *
 * @return True if the value is shared.
 */
bool AliYAMLConfiguration::IsSharedValue(std::string & value) const
{
  std::string stringToFind = "sharedParameters:";
  std::size_t sharedParameterLocation = value.find(stringToFind);
  if (sharedParameterLocation != std::string::npos)
  {
    value.erase(sharedParameterLocation, sharedParameterLocation + stringToFind.length());
    return true;
  }
  // Return false otherwise
  return false;
}

/**
 * Print a particular configuration.
 *
 * @param[in] i Index of the YAML configuration.
 */
void AliYAMLConfiguration::PrintConfiguration(const unsigned int i) const
{
  if (fConfigurations.size() > i)
  {
    auto & configPair = fConfigurations.at(i);
    AliInfoStream() << "\nName: " << configPair.first << "\n\n";
    AliInfoStream() << configPair.second << "\n";
  }
}

/**
 * Print a particular configuration.
 *
 * @param[in] name Name of the YAML configuration.
 */
void AliYAMLConfiguration::PrintConfiguration(const std::string & name) const
{
  PrintConfiguration(GetConfigurationIndexByName(name, fConfigurations));
}

/**
 * Print all configurations.
 */
void AliYAMLConfiguration::PrintConfigurations() const
{
  for (unsigned int i = 0; i < fConfigurations.size(); i++)
  {
    PrintConfiguration(i);
  }
}
