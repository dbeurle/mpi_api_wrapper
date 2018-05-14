pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
    }

  }
  stages {
    stage('build') {
      agent {
        dockerfile {
          filename 'Dockerfile.build'
        }

      }
      steps {
        sh 'ls; pwd; mkdir build && cd build && uname -a  && cmake ..'
      }
    }
  }
}