#ifndef example_oatpp_mongo_Database_hpp
#define example_oatpp_mongo_Database_hpp

#include "dto/DTOs.hpp"
#include "Model.hpp"

#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

#include <mongocxx/pool.hpp>
#include <bsoncxx/document/value.hpp>

namespace db {

class Database {
private:
  std::shared_ptr<mongocxx::pool> m_pool;
  std::string m_databaseName;
  std::string m_collectionName;
  oatpp::mongo::bson::mapping::ObjectMapper m_objectMapper;
private:
  oatpp::Object<AudioSample> audioSampleFromDto(const oatpp::Object<AudioMetaDto>& dto);
  oatpp::Object<AudioMetaDto> dtoFromAudioMeta(const oatpp::Object<AudioSample>& sample);
private:
  bsoncxx::document::value createMongoDocument(const oatpp::Void &polymorph);
public:

  Database(const mongocxx::uri &uri, const std::string &dbName, const std::string &collectionName);

  oatpp::Object<AudioMetaDto> createSample(const oatpp::Object<AudioMetaDto> &audioMetaDto);
  oatpp::Object<AudioMetaDto> updateSample(const oatpp::Object<AudioMetaDto> &audioMetaDto);
  oatpp::Object<AudioMetaDto> getSample(const oatpp::String& id);
  oatpp::List<oatpp::Object<AudioMetaDto>> getAllSamples();

  bool deleteSample(const oatpp::String& _id);

};

}

#endif //example_oatpp_mongo_Database_hpp