#include <gtkmm.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <iostream>
#include <vector>

class CameraWindow : public Gtk::Window
{
public:
    CameraWindow() : layout()
    {
        set_title("Basic application");

        set_child(overlay);
        overlay.add_overlay(area);

        toggle.set_valign(Gtk::Align::END);
        layout.append(toggle);
        
        layout.set_halign(Gtk::Align::CENTER);
        layout.set_margin_bottom(40);
        overlay.add_overlay(layout);
        toggle.set_label("");

        auto css = Gtk::CssProvider::create();
        css->load_from_path("./styletest.css");
        toggle.get_style_context()->add_provider(css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        toggle.get_style_context()->add_class("circular");
        
        area.set_draw_func([this](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
        {
            using namespace std;
            if(!camera.isOpened()) camera.open(0);

            cv::UMat image;
            camera >> image;
            if(image.empty()) return;
            
            auto factor = 1.0f;

            if(width < height) factor = width < image.cols ? 1.0 * width / image.cols : 1.0 * image.cols / width;
            else               factor = height < image.rows ? 1.0 * height / image.rows : 1.0 * image.rows / height;
            if(!factor) return;

            cr->set_source_rgb(40 / 255.,40 / 255.,40 / 255.);
            cr->paint();

            cv::resize(image,image,{0,0},factor,factor);
            cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
            auto img = Gdk::Pixbuf::create_from_data(image.getMat(cv::ACCESS_READ).data,Gdk::Colorspace::RGB,false,8,image.cols,image.rows,image.step);
            Gdk::Cairo::set_source_pixbuf(cr, img,(width - img->get_width()) / 2,(height - img->get_height()) / 2);
            cr->paint();
        });

        timer = Glib::signal_timeout().connect([this]()
        {       
            area.queue_draw();
            return true;
        },100);
    }

    ~CameraWindow() 
    {
        timer.disconnect();
    }

private:
    cv::VideoCapture camera;
    
    Gtk::Overlay overlay;    
    Gtk::Box layout;
    Gtk::DrawingArea area;
    Gtk::Button toggle;
    sigc::connection timer;
};

int main(int argc, char* argv[])
{
    // cv::Mat markerImage; 
    // // Load the predefined dictionary 
    // cv::Ptr<cv::aruco::Dictionary>dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250); 
    // // Generate the marker 
    // cv::aruco::drawMarker(dictionary, 33, 200, markerImage, 1);
    // cv::imwrite("xx.png",markerImage);

    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    return app->make_window_and_run<CameraWindow>(argc, argv);
    return 0;
}

//g++ app.cpp `pkg-config gtkmm-4.0 opencv4 --cflags --libs` && ./a.exe
