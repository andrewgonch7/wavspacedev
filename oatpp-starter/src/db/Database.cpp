#include "Database.hpp"

#include "db/Model.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"

#include <mongocxx/client.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <iostream>

namespace db {

Database::Database(const mongocxx::uri &uri, const std::string &dbName, const std::string &collectionName)
  : m_pool(std::make_shared<mongocxx::pool>(uri)), m_databaseName(dbName), m_collectionName(collectionName)
{}


oatpp::Object<AudioSample> Database::audioSampleFromDto(const oatpp::Object<AudioMetaDto>& dto) {
  auto sample = AudioSample::createShared();
  sample->_id = dto->_id;
  sample->numTransients = dto->numTransients;
  sample->url = dto->url;
  sample->artist = dto->artist;
  sample->albumPack = dto->albumPack;
  sample->sampleName = dto->sampleName;
  return sample;
}

oatpp::Object<AudioMetaDto> Database::dtoFromAudioMeta(const oatpp::Object<AudioSample>& sample) {
  auto dto = AudioMetaDto::createShared();
  dto->_id = sample->_id;
  dto->numTransients = sample->numTransients;
  dto->url = sample->url;
  dto->artist = sample->artist;
  dto->albumPack = sample->albumPack;
  dto->sampleName = sample->sampleName;
  return dto;
}

bsoncxx::document::value Database::createMongoDocument(const oatpp::Void &polymorph) {
  // if you have huge docs, you may want to increase starting BufferOutputStream size.
  // Or you may want to use oatpp::data::stream::ChunkedBuffer instead - for no-copy growth.
  oatpp::data::stream::BufferOutputStream stream;
  m_objectMapper.write(&stream, polymorph);
  bsoncxx::document::view view(stream.getData(), stream.getCurrentPosition());
  return bsoncxx::document::value(view);
}

oatpp::Object<AudioMetaDto> Database::createSample(const oatpp::Object<AudioMetaDto> &audioMetaDto) {
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];
  collection.insert_one(createMongoDocument(audioSampleFromDto(audioMetaDto)));
  
  return audioMetaDto;
}

oatpp::Object<AudioMetaDto> Database::updateSample(const oatpp::Object<AudioMetaDto> &audioMetaDto) {
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  collection.update_one(
    createMongoDocument( // <-- Filter
      oatpp::Fields<oatpp::String>({
        {"_id", audioMetaDto->_id}
      })
    ),
    createMongoDocument( // <-- Set
      oatpp::Fields<oatpp::Any>({ // map
        { // pair
          "$set", oatpp::Fields<oatpp::Any>({ // you can also define a "strict" DTO for $set operation.
            {"numTransients", audioMetaDto->numTransients},
            {"url", audioMetaDto->url},
            {"artist", audioMetaDto->artist},
            {"albumPack", audioMetaDto->albumPack},
            {"sampleName", audioMetaDto->sampleName}
          })
        } // pair
      }) // map
    )
  );

  return audioMetaDto;
}

oatpp::Object<AudioMetaDto> Database::getSample(const oatpp::String& _id) {
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto result =
    collection.find_one(createMongoDocument( // <-- Filter
      oatpp::Fields<oatpp::String>({
        {"_id", _id}
      })
    ));

  if(result) {
    auto view = result->view();
    auto bson = oatpp::String((const char*)view.data(), view.length());
    auto sample = m_objectMapper.readFromString<oatpp::Object<AudioSample>>(bson);
    return dtoFromAudioMeta(sample);
  }

  return nullptr;
}

oatpp::List<oatpp::Object<AudioMetaDto>> Database::getAllSamples() {
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto cursor = collection.find(
    createMongoDocument(oatpp::Fields<oatpp::String>({})
  ));

  oatpp::List<oatpp::Object<AudioMetaDto>> list({});

  for(auto view : cursor) {
    auto bson = oatpp::String((const char*)view.data(), view.length());
    auto sample = m_objectMapper.readFromString<oatpp::Object<AudioSample>>(bson);
    list->push_back(dtoFromAudioMeta(sample));
  }

  return list;

}

bool Database::deleteSample(const oatpp::String& _id) {
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_databaseName][m_collectionName];

  auto result =
    collection.delete_one(createMongoDocument( // <-- Filter
      oatpp::Fields<oatpp::String>({
        {"_id", _id}
      })
    ));

  if(result) {
    return result->deleted_count() == 1;
  }
  return false;
}

}