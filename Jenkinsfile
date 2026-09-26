// Atelier3D CI pipeline
// Builds the desktop app (CMake project in apps/desktop) on every change to main and reports failures.
pipeline {
    // Runs on the Ubuntu Jenkins node
    // (needs build-essential, cmake, ninja-build, libgl1-mesa-dev, xorg-dev, mingw-w64, cppcheck,
    //  clang-format, clang-tidy)
    // Jenkins plugins: Pipeline, Git, Timestamper, JUnit, Warnings (warnings-ng)
    // Must run on the Jenkins controller (built-in node): Publish Downloads writes to $JENKINS_HOME/userContent
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
                // GoogleTest suite in tests/unit; any failing test fails the build
                sh 'bash tools/ci/test.sh Debug'
            }
        }

        stage('Lint') {
            steps {
                // clang-format (code style, .clang-format) and clang-tidy (code lint, .clang-tidy) on our code;
                // fails on unformatted files or bugprone/analyzer findings, reports the rest
                sh 'bash tools/ci/lint.sh Debug'
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
                // Downloadable from the build page under "Build Artifacts" (team only: needs Job/Read)
                archiveArtifacts artifacts: 'apps/desktop/build/dist/*.tar.gz, apps/desktop/build/dist/*.zip', fingerprint: true
            }
        }

        stage('Publish Downloads') {
            steps {
                // Public downloads for guests, who only need Overall/Read (no Job/Read, so no job page,
                // stage view or trends): <jenkins>/userContent/atelier3d/ (history page with download links)
                sh 'bash tools/ci/publish-downloads.sh "$JENKINS_HOME/userContent/atelier3d" "$BUILD_NUMBER"'
            }
        }
    }

    post {
        always {
            // Test results: "Test Result" page and trend chart on the job page
            junit testResults: 'apps/desktop/build/reports/tests.xml', allowEmptyResults: true

            // Compiler warnings (from the console log), cppcheck, clang-tidy and clang-format findings:
            // "GCC Warnings", "CPPCheck Warnings", "Clang-Tidy Warnings" and "clang-format" pages and trend charts
            recordIssues enabledForFailure: true,
                         tools: [gcc(), cppCheck(pattern: 'apps/desktop/build/reports/cppcheck.xml'),
                                 clangTidy(pattern: 'apps/desktop/build/reports/clang-tidy.txt'),
                                 clang(pattern: 'apps/desktop/build/reports/clang-format.txt', id: 'clang-format', name: 'clang-format')],
                         filters: [excludeFile('.*/_deps/.*')]

            // Public build history (every build, pass or fail); never fails the build itself
            sh "bash tools/ci/publish-history.sh \"\$JENKINS_HOME/userContent/atelier3d\" \"\$BUILD_NUMBER\" ${currentBuild.currentResult} " +
               "|| echo 'WARNING: could not update the public build history'"
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
