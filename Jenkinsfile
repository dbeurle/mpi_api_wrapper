pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull --no-cache'
    }

  }
  stages {
    stage('build') {
      steps {
        sh 'ls; mkdir build && cd build && source /etc/profile.d/modules.sh && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
  }
}
