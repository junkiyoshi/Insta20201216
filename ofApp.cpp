#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofEnableDepthTest();

	this->cap_size = cv::Size(960, 540);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->cap.open("D:\\MP4\\video9.mp4");
	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}

	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 50, true, true, true);
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_index = (int)(ofGetFrameNum() * 0.42) % this->number_of_frames;
	this->frame_list[frame_index].copyTo(this->frame);
	this->image.update();

	this->mesh_list.clear();

	float radius = 200;
	int deg_span = 3;
	int x_span = 4;
	for (int x = 0; x < this->cap_size.width; x += x_span) {

		ofMesh mesh, frame;
		frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
		int deg_start = ofMap(ofNoise(x * 0.0005, ofGetFrameNum() * 0.005), 0, 1, -360, 360);
		for (int deg = deg_start; deg < deg_start + 180; deg += deg_span) {

			int index = mesh.getNumVertices();

			vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, radius* cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD)));

			mesh.addVertices(vertices);
			frame.addVertices(vertices);

			int y = ofMap(deg - deg_start, 0, 180, 0, this->cap_size.height);
			int next_y = ofMap(deg + deg_span - deg_start, 0, 180, 0, this->cap_size.height);

			mesh.addTexCoord(glm::vec3(x, y, 0));
			mesh.addTexCoord(glm::vec3(x, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, y, 0));

			mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
			mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);
		}

		this->mesh_list.push_back(mesh);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	for (int i = 0; i < this->mesh_list.size(); i++) {

		float z = (this->mesh_list[i].getVertex(0).z + this->mesh_list[i].getVertex(2).z) / 2;

		this->image.bind();
		ofSetColor(255);
		this->mesh_list[i].draw();
		this->image.unbind();
	}

	ofRotateX(180);

	auto word = "Achieved for 4 years.";
	auto base_location = glm::vec2(this->font.stringWidth(word) * -0.5, this->font.stringHeight(word) * 0.5);
	auto param = ofGetFrameNum();

	vector<ofPath> word_path = this->font.getStringAsPoints(word, true, false);
	for (int word_index = 0; word_index < word_path.size(); word_index++) {

		vector<ofPolyline> outline = word_path[word_index].getOutline();

		ofFill();
		ofSetColor(239);
		ofBeginShape();
		for (int outline_index = 0; outline_index < outline.size(); outline_index++) {

			if (outline_index != 0) { ofNextContour(true); }

			vector<glm::vec3> vertices = outline[outline_index].getVertices();
			for (auto& vertex : vertices) { vertex += glm::vec3(base_location, 0); }

			ofVertices(vertices);
		}
		ofEndShape();
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}