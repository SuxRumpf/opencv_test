#include "../include/helper_functions.h"
#include <gst/gst.h>


void gstreamer_test_1c(int argc, char *argv[]){
    std::cout << "\n\n---GStreamer Test ---" << std::endl;
    //gst-launch-1.0 udpsrc port=9002 ! application/x-rtp,encoding-name=H265,payload=96 ! rtph265depay ! h265parse ! avdec_h265 ! xvimagesink

    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Build the pipeline */
    pipeline =
            gst_parse_launch
                    ("playbin uri=https://gstreamer.freedesktop.org/documentation/tutorials/basic/hello-world.html",
                     NULL);

    /* Start playing */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    /* GST_MESSAGE_ERROR | GST_MESSAGE_EOS durch (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS) ersetzen
     * https://stackoverflow.com/questions/14923306/gstreamer-with-visual-c-express-2010-tutorial-1
    */
    msg =
            gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
        g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
                 "variable set for more details.");
    }

    /* Free resources */
    gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

}

int gstreamer_test_2c(int argc, char *argv[]){
    GstElement *pipeline, *source, *conv,*enc, *pay, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Create the elements */
    source = gst_element_factory_make ("v4l2src", "source");

    conv = gst_element_factory_make ("videoconvert", "conv");

    enc = gst_element_factory_make("x264enc", "enc");

    pay = gst_element_factory_make("rtph264pay", "pay");
    g_object_set(G_OBJECT(pay), "config-interval", 1, NULL);

    sink = gst_element_factory_make ("udpsink", "sink");

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new ("test-pipeline");

    if (!pipeline || !source || !sink) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many (GST_BIN (pipeline), source, conv, enc, pay, sink, NULL);
    if (gst_element_link_many (source, conv, enc, pay, sink, NULL) != TRUE) {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }

    /* Modify the source's properties host=196.0.0.2 port=9002*/
    g_object_set (source, "device", "/dev/video0", NULL);
    g_object_set (sink, "host", "196.0.0.2", NULL);
    g_object_set (sink, "port", 9002, NULL);


    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    msg =
            gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error (msg, &err, &debug_info);
                g_printerr ("Error received from element %s: %s\n",
                            GST_OBJECT_NAME (msg->src), err->message);
                g_printerr ("Debugging information: %s\n",
                            debug_info ? debug_info : "none");
                g_clear_error (&err);
                g_free (debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print ("End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr ("Unexpected message received.\n");
                break;
        }
        gst_message_unref (msg);
    }

    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}

int main(int argc, char *argv[]){
    gstreamer_test_2c(argc, argv);

    return 0;
}