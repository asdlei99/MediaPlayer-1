package com.jazzros.filelist;

import android.content.ContentResolver;
import android.content.ContentUris;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
/**
 * Retrieves and organizes media to play. Before being used, you must call {@link #prepare()},
 * which will retrieve all of the music on the user's device (by performing a query on a content
 * resolver). After that, it's ready to retrieve a random song, with its title and URI, upon
 * request.
 */
public class MediaRetriever
{
    final String TAG = "MediaRetriever";
    ContentResolver mContentResolver;
    // the items (songs) we have queried
    List<Item> mItems = new ArrayList<Item>();
    Random mRandom = new Random();

    public MediaRetriever(ContentResolver cr) {
        mContentResolver = cr;
    }

    /**
     * Loads music data. This method may take long, so be sure to call it asynchronously without
     * blocking the main thread.
     */
    public void prepare() {
///        Uri uri = android.provider.MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
        Uri uri = android.provider.MediaStore.Video.Media.EXTERNAL_CONTENT_URI;

        Log.i(TAG, "Querying media...");
        Log.i(TAG, "URI: " + uri.toString());
        // Perform a query on the content resolver. The URI we're passing specifies that we
        // want to query for all audio media on external storage (e.g. SD card)
        Cursor cur = mContentResolver.query(uri, null,
                null, // MediaStore.Video.Media.IS_MUSIC + " = 1",
                null, null);
        Log.i(TAG, "Query finished. " + (cur == null ? "Returned NULL." : "Returned a cursor."));
        if (cur == null) {
            // Query failed...
            Log.e(TAG, "Failed to retrieve music: cursor is null :-(");
            return;
        }
        if (!cur.moveToFirst()) {
            // Nothing to query. There is no music on the device. How boring.
            Log.e(TAG, "Failed to move cursor to first row (no query results).");
            return;
        }
        Log.i(TAG, "Listing...");
        // retrieve the indices of the columns where the ID, title, etc. of the song are
        int artistColumn = cur.getColumnIndex(MediaStore.Video.Media.ARTIST);
        int titleColumn = cur.getColumnIndex(MediaStore.Video.Media.TITLE);
        int albumColumn = cur.getColumnIndex(MediaStore.Video.Media.ALBUM);
        int durationColumn = cur.getColumnIndex(MediaStore.Video.Media.DURATION);
        int idColumn = cur.getColumnIndex(MediaStore.Video.Media._ID);
        int dataColumnIndex = cur.getColumnIndex(MediaStore.Video.Media.DATA);

        Log.i(TAG, "Title column index: " + String.valueOf(titleColumn));
        Log.i(TAG, "ID column index: " + String.valueOf(titleColumn));
        // add each song to mItems
        do {
            Log.i(TAG, "ID: " + cur.getString(idColumn) + " Title: " + cur.getString(titleColumn));

            Bitmap thumbnail = MediaStore.Video.Thumbnails.getThumbnail(
                                mContentResolver, cur.getLong(idColumn),
                                MediaStore.Video.Thumbnails.MICRO_KIND, null);

            mItems.add(new Item(
                    thumbnail,
                    cur.getString(dataColumnIndex),
                    cur.getLong(idColumn),
                    cur.getString(artistColumn),
                    cur.getString(titleColumn),
                    cur.getString(albumColumn),
                    cur.getLong(durationColumn)));
        } while (cur.moveToNext());
        Log.i(TAG, "Done querying media. MediaRetriever is ready.");
    }
    public ContentResolver getContentResolver() {
        return mContentResolver;
    }
    /** Returns a random Item. If there are no items available, returns null. */
    public Item getRandomItem() {
        if (mItems.size() <= 0) return null;
        return mItems.get(mRandom.nextInt(mItems.size()));
    }
    public List<Item> getItems() {
        return mItems;
    }
    public Item getItem(int index) {
        return mItems.get(index);
    }
    public int getSize() {
        return mItems.size();
    }
    public static class Item {
        Bitmap thumbnail;
        String path;
        long id;
        String artist;
        String title;
        String album;
        long duration;

        public Item(Bitmap thumb, String path, long id, String artist, String title, String album, long duration) {
            this.thumbnail = thumb;
            this.path = path;
            this.id = id;
            this.artist = artist;
            this.title = title;
            this.album = album;
            this.duration = duration;
        }
        public long getId() {
            return id;
        }
        public String getArtist() {
            return artist;
        }
        public String getTitle() {
            return title;
        }
        public String getAlbum() {
            return album;
        }
        public long getDuration() {
            return duration;
        }
        public Uri getURI() {
            return ContentUris.withAppendedId(
                    android.provider.MediaStore.Video.Media.EXTERNAL_CONTENT_URI, id);
        }
        public Bitmap getThumbnail() {
            return thumbnail;
        }
        public String getPath() {
            return path;
        }
    }
}