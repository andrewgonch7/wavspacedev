#ifndef MyController_hpp
#define MyController_hpp

#include "db/Database.hpp"
#include "dto/DTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include <ctime>
#include <unistd.h>

//======= Transient Detection Dependencies =======
#include "../audiolib/AudioFile.h"
#include "../audiolib/AudioData.h"
#include "../audiolib/TransientDetector.h"
//================================================

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController {
private:
  OATPP_COMPONENT(std::shared_ptr<db::Database>, m_database);
public:
  /**
   * Constructor with object mapper.
   * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
   */
  MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<MyController> createShared(
    OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
  {
    return std::make_shared<MyController>(objectMapper);
  }
  
  ENDPOINT("GET", "/", root) {
    auto dto = MyDto::createShared();
    dto->statusCode = 200;
    dto->message = "Hello World!";
    return createDtoResponse(Status::CODE_200, dto);
  }
  
  ENDPOINT("GET", "/audio", getAudio) {
    auto dto = AudioDto::createShared();
    dto->statusCode = 200;
    dto->numTransients = 2;
    return createDtoResponse(Status::CODE_200, dto);
  }

  std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
  }

  int getTransients(std::string& file) {
    
    
      AudioFile<double> audioFile;
      bool isLoaded = audioFile.load(file);
      if (!isLoaded) {
          std::cout << "Failed to load file " << std::endl;
          return -1;
      }
      AudioData audio(audioFile.samples[0]);
      Signal::TransientDetector td(audioFile.getSampleRate());
      td.SetValleyToPeakRatio(1.0);
      std::vector<std::size_t> transients;
      const AudioData audioBuffer(audio);
      td.FindTransients(audioBuffer, transients);
      
      std::cout << "Number of transients for test: " << transients.size() << std::endl;
      return transients.size();
    
  }

  /**
   * @brief Processes audio and detects number of transients in the audio file
   * also uploads the audio file into object storage
   * @return numTransients, url (object storage link)
   */
  ENDPOINT("POST", "/processAudio", processAudio, REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string random = gen_random(7);
    char file[27] = {'.', '.', '/', 's', 'r', 'c', '/', 'u', 'p', 'l', 'o', 'a', 'd', 's', '/', random[0],
    random[1], random[2], random[3], random[4], random[5], random[6], '.', 'w', 'a', 'v', '\0'};
    oatpp::data::stream::FileOutputStream fileOutputStream(file);
    request->transferBodyToStream(&fileOutputStream); // transfer body chunk by chunk

    std::string filepath = file;
    //std::cout << "File path is " << filepath << std::endl;
    int numTransients = getTransients(filepath);
    if (numTransients == -1) {
      // Failed to load the audio file in memory
      return createResponse(Status::CODE_422);
    }
    std::remove("../src/uploads/test8.wav");
    auto dto = AudioDto::createShared();
    dto->statusCode = 201;
    dto->numTransients = numTransients;
    dto->url = "s3.example.com"; // TODO
    return createDtoResponse(Status::CODE_200, dto);
  } 


  ENDPOINT("PUT", "/upload", upload, BODY_DTO(Object<AudioMetaDto>, audioMetaDto)) {

    auto sample = m_database->createSample(audioMetaDto);
    OATPP_ASSERT_HTTP(sample, Status::CODE_404, "Database failed");
    auto dto = HTTP_Response::createShared();
    dto->statusCode = 201;
    dto->message = "Uploaded sample to database successfully";
    return createDtoResponse(Status::CODE_201, dto);
  } 
  
  // Demo Write Process
  // 1. Upload Binary data - process -> validate -> get S3 url -> return numTransients & url in response
  // 2. Upload meta-data

  // Production Write Process
  //  1. Upload request is made -> all blob & metadata put in temporary storage somewhere
  //  2. Admin looks at data and validates if it is okay
  //  3. If valid, upload file in s3 and production databases

  // TODO Insert Your endpoints here !!!

};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif /* MyController_hpp */
