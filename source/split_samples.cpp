#include <DataTypes.hpp>
#include <CMDParser.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <algorithm> // std::shuffle
#include <fstream>
#include <iostream>
#include <unistd.h>


int main(int argc, char* argv[]){
  bool shuffle = false;
  CMDParser p("input outputA outputB");
  p.addOpt("s",-1,"shuffle", "shuffle and split, default: false");
  p.init(argc,argv);

  if(p.getArgs().size() != 3){
    p.showHelp();
  }

  if(p.isOptSet("s")){
    shuffle = true;
  }


  
  std::ifstream iff(p.getArgs()[0].c_str(), std::ifstream::binary);
  std::ofstream off1(p.getArgs()[1].c_str(), std::ifstream::binary);
  std::ofstream off2(p.getArgs()[2].c_str(), std::ifstream::binary);
  std::ofstream* off = 0;
  const unsigned num_samples_in_file = calcNumFrames(iff,
						     sizeof(float) +
						     sizeof(uv) +
						     sizeof(uv) +
						     sizeof(xyz) +
						     sizeof(uv) +
						     sizeof(glm::vec3) +
						     sizeof(float));

  // two methods
  if(!shuffle){
    const unsigned stride = CB_WIDTH * CB_HEIGHT;
    bool to_off1 = false;

    for(unsigned i = 0; i < num_samples_in_file; ++i){

      samplePoint s;
      iff.read((char*) &s.depth, sizeof(float));
      iff.read((char*) &s.tex_color, sizeof(uv));
      iff.read((char*) &s.tex_depth, sizeof(uv));
      iff.read((char*) &s.pos_offset, sizeof(xyz));
      iff.read((char*) &s.tex_offset, sizeof(uv));
      iff.read((char*) glm::value_ptr(s.pos_real), sizeof(glm::vec3));
      iff.read((char*) &s.quality, sizeof(float));
      
      if(i % stride == 0){
	to_off1 = !to_off1;
      }
      off = to_off1 ? &off1 : &off2;
      
      off->write((const char*) &s.depth, sizeof(float));
      off->write((const char*) &s.tex_color, sizeof(uv));
      off->write((const char*) &s.tex_depth, sizeof(uv));
      off->write((const char*) &s.pos_offset, sizeof(xyz));
      off->write((const char*) &s.tex_offset, sizeof(uv));
      off->write((const char*) glm::value_ptr(s.pos_real), sizeof(glm::vec3));
      off->write((const char*) &s.quality, sizeof(float));
      
    }
  }
  else{
    std::vector<samplePoint> sps;
    for(unsigned i = 0; i < num_samples_in_file; ++i){

      samplePoint s;
      iff.read((char*) &s.depth, sizeof(float));
      iff.read((char*) &s.tex_color, sizeof(uv));
      iff.read((char*) &s.tex_depth, sizeof(uv));
      iff.read((char*) &s.pos_offset, sizeof(xyz));
      iff.read((char*) &s.tex_offset, sizeof(uv));
      iff.read((char*) glm::value_ptr(s.pos_real), sizeof(glm::vec3));
      iff.read((char*) &s.quality, sizeof(float));
      
      sps.push_back(s);
    }

    std::shuffle(std::begin(sps), std::end(sps), std::default_random_engine());
    unsigned i = 0;
    bool to_off1 = false;
    for(const samplePoint& s : sps){
      if(i % 2 == 0){
	to_off1 = !to_off1;
      }
      ++i;

      off = to_off1 ? &off1 : &off2;
      
      off->write((const char*) &s.depth, sizeof(float));
      off->write((const char*) &s.tex_color, sizeof(uv));
      off->write((const char*) &s.tex_depth, sizeof(uv));
      off->write((const char*) &s.pos_offset, sizeof(xyz));
      off->write((const char*) &s.tex_offset, sizeof(uv));
      off->write((const char*) glm::value_ptr(s.pos_real), sizeof(glm::vec3));
      off->write((const char*) &s.quality, sizeof(float));

    }

  }
  iff.close();
  off1.close();
  off2.close();

  return 0;
}