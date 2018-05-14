pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
      additionalBuildArgs '--pull --no-cache'
    }
    options { newContainerPerStage() }
  }
  stages {
    stage('build') {
      steps {
        sh 'cat Jenkinsfile && ls; mkdir build && cd build && source /etc/profile.d/modules.sh && module load mpi/openmpi-x86_64  && cmake .. && make all -j4'
      }
    }
  }
}
