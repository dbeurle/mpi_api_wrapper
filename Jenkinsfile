pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      label 'fedora_builder'
      additionalBuildArgs '--no-cache'
    }

  }
  stages {
    stage('build') {
      agent any
      steps {
        sh 'ls; mkdir build && cd build && source /etc/profile.d/modules.sh && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
  }
}
