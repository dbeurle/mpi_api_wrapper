pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull'
    }

  }
  stages {
    stage('configure debug') {
      steps {
        sh 'source /etc/profile.d/modules.sh &&                     module load mpi/openmpi-x86_64 &&                     if [ ! -d "build" ]; then mkdir build; fi &&                     cd build &&                     rm -rf * && cmake -DCMAKE_BUILD_TYPE=Debug ..'
      }
    }
    stage('build') {
      steps {
        sh 'cd build && make all'
      }
    }
    stage('test') {
      steps {
        sh 'cd build && ctest'
      }
    }
  }
}