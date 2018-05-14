pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull'
    }

  }
  stages {
    stage('openmpi module') {
      steps {
        sh 'source /etc/profile.d/modules.sh &&                     module load mpi/openmpi-x86_64'
      }
    }
    stage('build') {
      steps {
        sh 'if [ ! -d "build" ]; then mkdir build; fi && cd build && make all'
      }
    }
    stage('test') {
      steps {
        sh 'cd build && ctest'
      }
    }
  }
}