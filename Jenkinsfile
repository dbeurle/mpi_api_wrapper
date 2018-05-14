pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull'
    }

  }
  stages {
    stage('configure') {
      steps {
        sh 'source /etc/profile.d/modules.sh &&                     module load mpi/openmpi-x86_64 &&                     if [ ! -d "build" ]; then mkdir build; fi &&                     cd build &&                     rm -rf *'
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