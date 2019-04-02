package com.wisc.xsscookiestealing;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.webkit.CookieManager;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //set the layout file
        setContentView(R.layout.activity_main);
        //store the URL of the website generated by local server
        String domain = "http://10.0.2.2:8080";

        //create a webview object
        WebView web;
        //create a CookieManager object to od operations on cookies
        final CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);
        cookieManager.removeAllCookies(null);
        //set the cookies of the domain
        cookieManager.setCookie(domain, "session = 1234;");
        //enable cookies
        CookieManager.getInstance().setAcceptCookie(true);

        web = (WebView)findViewById(R.id.web1);
        //enable javascript code running on webview
        web.getSettings().setJavaScriptEnabled(true);

        //present the desired URL
        web.loadUrl(domain);

        //by default, alert() does not respond in webview;
        web.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onJsAlert(WebView view, String url, String message,
                                     JsResult result) {
                return super.onJsAlert(view, url, message, result);
            }
        });

        //You may need to change this method for mitigation
        web.setWebViewClient(new WebViewClient());

    }
}

