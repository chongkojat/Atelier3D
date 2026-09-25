// Atelier3D CI pipeline
// Builds the CMake project in Source/ on every change to main and reports failures.
pipeline {
    // Runs on the Ubuntu Jenkins node
    // (needs build-essential, cmake, ninja-build, libgl1-mesa-dev, xorg-dev, mingw-w64)
    agent any

    options {
        timestamps()
        buildDiscarder(logRotator(numToKeepStr: '20'))
        disableConcurrentBuilds()
        timeout(time: 60, unit: 'MINUTES')
    }

    triggers {
        // Checks GitHub for new commits every ~5 minutes; works without a public webhook URL
        pollSCM('H/5 * * * *')
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
                sh 'git log -1 --oneline'
            }
        }

        stage('Build Linux (Debug)') {
            steps {
                sh 'bash tools/ci/build.sh Debug linux'
            }
        }

        stage('Build Windows (Release)') {
            steps {
                // Cross-compiles native Windows .exe files with MinGW-w64
                sh 'bash tools/ci/build.sh Release windows'
            }
        }

        stage('Package Windows') {
            steps {
                sh 'bash tools/ci/package-windows.sh Release'
                archiveArtifacts artifacts: 'build/dist/*.zip', fingerprint: true
            }
        }
    }

    post {
        success {
            echo "BUILD PASSED: ${env.JOB_NAME} #${env.BUILD_NUMBER}"
        }
        failure {
            echo "BUILD FAILED: ${env.JOB_NAME} #${env.BUILD_NUMBER} - see ${env.BUILD_URL}console"
            // Optional: uncomment once SMTP is set up in Manage Jenkins > System
            // mail to: 'team@example.com',
            //      subject: "FAILED: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
            //      body: "Build failed on main. Console log: ${env.BUILD_URL}console"
        }
    }
}
