// Atelier3D CI pipeline
// Builds the CMake project in Source/ on every change to main and reports failures.
pipeline {
    // Runs on the Ubuntu Jenkins node
    // (needs build-essential, cmake, ninja-build, libgl1-mesa-dev, xorg-dev, mingw-w64, cppcheck)
    // Jenkins plugins: Pipeline, Git, Timestamper, JUnit, Warnings (warnings-ng)
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

        stage('Unit Tests') {
            steps {
                // GoogleTest suite in Source/tests; any failing test fails the build
                sh 'bash tools/ci/test.sh Debug'
            }
        }

        stage('Static Analysis') {
            steps {
                // cppcheck on our code; fails on "error" severity, reports the rest
                sh 'bash tools/ci/static-analysis.sh'
            }
        }

        stage('Build Linux (Release)') {
            steps {
                sh 'bash tools/ci/build.sh Release linux'
            }
        }

        stage('Build Windows (Release)') {
            steps {
                // Cross-compiles native Windows .exe files with MinGW-w64
                sh 'bash tools/ci/build.sh Release windows'
            }
        }

        stage('Package') {
            steps {
                sh 'bash tools/ci/package.sh Release linux'
                sh 'bash tools/ci/package.sh Release windows'
                // Downloadable from the build page under "Build Artifacts"
                archiveArtifacts artifacts: 'build/dist/*.tar.gz, build/dist/*.zip', fingerprint: true
            }
        }
    }

    post {
        always {
            // Test results: "Test Result" page and trend chart on the job page
            junit testResults: 'build/reports/tests.xml', allowEmptyResults: true

            // Compiler warnings (from the console log) and cppcheck findings:
            // "GCC Warnings" and "CPPCheck Warnings" pages and trend charts
            recordIssues enabledForFailure: true,
                         tools: [gcc(), cppCheck(pattern: 'build/reports/cppcheck.xml')],
                         filters: [excludeFile('.*/_deps/.*')]
        }
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
