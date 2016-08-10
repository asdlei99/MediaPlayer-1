// src: https://vikaskanani.wordpress.com/2011/07/20/android-custom-image-gallery-with-checkbox-in-grid-to-select-multiple/
package com.jazzros.filelist;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.Toast;

import com.jazzros.ffmpegtest.R;

import java.util.ArrayList;
import java.util.List;

public class GalleryPage extends Activity {
    MediaRetriever  mediaRetriever;
//    private int count;
//    private Bitmap[] thumbnails;
    private boolean[] thumbnailsselection;
//    private String[] title;
//    private String[] arrPath;
    private ImageAdapter imageAdapter;

    public static final String ACTPARAM_STRINGARRAY = "parStringArray";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.gallery);

        mediaRetriever = new MediaRetriever (getApplicationContext().getContentResolver());
        mediaRetriever.prepare();

        this.thumbnailsselection = new boolean[mediaRetriever.getSize()];

        imageAdapter = new ImageAdapter();
        GridView imagegrid = (GridView) findViewById(R.id.PhoneImageGrid);
        imagegrid.setAdapter(imageAdapter);

/*
        final String[] columns = { MediaStore.Video.Media.DATA, MediaStore.Video.Media._ID };
        final String orderBy = MediaStore.Video.Media._ID;
        Cursor imagecursor =managedQuery(
                MediaStore.Video.Media.EXTERNAL_CONTENT_URI, columns, null,
                null, orderBy);
        int image_column_index = imagecursor.getColumnIndex(MediaStore.Video.Media._ID);
        this.count = Math.min(imagecursor.getCount(), 50);
        this.thumbnails = new Bitmap[this.count];
        this.arrPath = new String[this.count];
        this.title = new String[this.count];
        this.thumbnailsselection = new boolean[this.count];

        for (int i = 0; i < this.count; i++) {
            imagecursor.moveToPosition(i);
            int id = imagecursor.getInt(image_column_index);
            int dataColumnIndex = imagecursor.getColumnIndex(MediaStore.Video.Media.DATA);
            thumbnails[i] = MediaStore.Video.Thumbnails.getThumbnail(
                    getApplicationContext().getContentResolver(), id,
                    MediaStore.Video.Thumbnails.MICRO_KIND, null);
            arrPath[i]= imagecursor.getString(dataColumnIndex);
        }
        GridView imagegrid = (GridView) findViewById(R.id.PhoneImageGrid);
        imageAdapter = new ImageAdapter();
        imagegrid.setAdapter(imageAdapter);
///        imagecursor.close();
*/

        final Button selectBtn = (Button) findViewById(R.id.selectBtn);
        selectBtn.setOnClickListener(new OnClickListener() {

            public void onClick(View v) {
                // TODO Auto-generated method stub
                final int len = thumbnailsselection.length;
                int cnt = 0;
                ArrayList<String> passParameterArray = new ArrayList<String>();
                String selectImages = "";
                for (int i =0; i<len; i++)
                {
                    if (thumbnailsselection[i]){
                        cnt++;
                        selectImages = selectImages + mediaRetriever.getItem(i).getPath() + "|";

                        passParameterArray.add(mediaRetriever.getItem(i).getPath());
                    }
                }
                passParameterArray.add (0, Integer.toString(cnt));
                passParameterArray.add("-i");
                passParameterArray.add(Integer.toString(1));
                passParameterArray.add("-s");
                passParameterArray.add(Integer.toString(5000));
                passParameterArray.add("-e");
                passParameterArray.add(Integer.toString(10000));
                passParameterArray.add("-i");
                passParameterArray.add(Integer.toString(0));
                passParameterArray.add("-s");
                passParameterArray.add(Integer.toString(5000));
                passParameterArray.add("-e");
                passParameterArray.add(Integer.toString(10000));
                passParameterArray.add("-i");
                passParameterArray.add(Integer.toString(1));
                passParameterArray.add("-s");
                passParameterArray.add(Integer.toString(10000));
                passParameterArray.add("-e");
                passParameterArray.add(Integer.toString(15000));
                if (cnt == 0){
                    Toast.makeText(getApplicationContext(),
                            "Please select at least one image",
                            Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(getApplicationContext(),
                            "You've selected Total " + cnt + " image(s).",
                            Toast.LENGTH_LONG).show();
                    Log.d("SelectedImages", selectImages);
                    //
                    //
                    //
                    Bundle b=new Bundle();
                    b.putStringArrayList(ACTPARAM_STRINGARRAY, passParameterArray);
                    Intent intent = new Intent(getBaseContext(), org.libsdl.app.SDLActivity.class);
                    intent.putExtras(b);
                    startActivity(intent);
                }
            }
        });
    }

    public class ImageAdapter extends BaseAdapter {
        private LayoutInflater mInflater;

        public ImageAdapter() {
            mInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        public int getCount() {
            return mediaRetriever.getSize();
        }

        public Object getItem(int position) {
            return position;
        }

        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;
            if (convertView == null) {
                holder = new ViewHolder();
                convertView = mInflater.inflate(
                        R.layout.galleryitem, null);
                holder.imageview = (ImageView) convertView.findViewById(R.id.thumbImage);
                holder.checkbox = (CheckBox) convertView.findViewById(R.id.itemCheckBox);

                convertView.setTag(holder);
            }
            else {
                holder = (ViewHolder) convertView.getTag();
            }
            holder.checkbox.setId(position);
            holder.imageview.setId(position);
            holder.checkbox.setOnClickListener(new OnClickListener() {

                public void onClick(View v) {
                    // TODO Auto-generated method stub
                    CheckBox cb = (CheckBox) v;
                    int id = cb.getId();
                    if (thumbnailsselection[id]){
                        cb.setChecked(false);
                        thumbnailsselection[id] = false;
                    } else {
                        cb.setChecked(true);
                        thumbnailsselection[id] = true;
                    }
                }
            });
            holder.imageview.setOnClickListener(new OnClickListener() {

                public void onClick(View v) {
                    // TODO Auto-generated method stub
                    int id = v.getId();
                    Intent intent = new Intent();
                    intent.setAction(Intent.ACTION_VIEW);
                    intent.setDataAndType(Uri.parse("file://" + mediaRetriever.getItem(id).getPath()), "image/*");
                    startActivity(intent);
                }
            });
            holder.imageview.setImageBitmap(mediaRetriever.getItem(position).getThumbnail());
            holder.checkbox.setChecked(thumbnailsselection[position]);
            holder.id = position;
            return convertView;
        }
    }
    class ViewHolder {
        ImageView imageview;
        CheckBox checkbox;
        int id;
    }
}