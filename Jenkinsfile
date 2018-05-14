pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
    }

  }
  stages {
    stage('build') {
      agent any
      steps {
        sh 'ls; pwd; mkdir build && cd build && cmake ..'
      }
    }
  }
}