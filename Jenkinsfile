pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile.build'
    }

  }
  stages {
    stage('print_garbage') {
      steps {
        sh 'echo "Hello world"'
      }
    }
  }
}